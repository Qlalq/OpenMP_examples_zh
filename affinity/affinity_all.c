#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <string.h>
#include <math.h>

// 矩阵大小 - 增大矩阵以便更好观察性能差异
#define N 2000
// 线程数固定为256
#define NUM_THREADS 256

// 用于时间测量
double get_time() {
    return omp_get_wtime();
}

// 初始化矩阵
void init_matrix(double **A, double **B, double **C, double **D) {
    // 分配内存
    *A = (double*)malloc(N * N * sizeof(double));
    *B = (double*)malloc(N * N * sizeof(double));
    *C = (double*)malloc(N * N * sizeof(double));
    *D = (double*)malloc(N * N * sizeof(double));
    
    // 初始化矩阵数据
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            (*A)[i*N + j] = (double)rand() / RAND_MAX;
            (*B)[i*N + j] = (double)rand() / RAND_MAX;
            (*C)[i*N + j] = 0.0;
            (*D)[i*N + j] = 0.0;
        }
    }
}

// 串行矩阵乘法
void serial_matrix_multiply(double *A, double *B, double *C) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

// 不使用亲和性的并行矩阵乘法
void parallel_matrix_multiply_no_affinity(double *A, double *B, double *C) {
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

// 使用close亲和性的并行矩阵乘法
void parallel_matrix_multiply_close(double *A, double *B, double *C) {
    #pragma omp parallel for num_threads(NUM_THREADS) proc_bind(close)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

// 使用spread亲和性的并行矩阵乘法
void parallel_matrix_multiply_spread(double *A, double *B, double *C) {
    #pragma omp parallel for num_threads(NUM_THREADS) proc_bind(spread)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

// 使用分块+亲和性优化的矩阵乘法
void parallel_matrix_multiply_tiled(double *A, double *B, double *C) {
    // 分块大小 - 对缓存友好
    int block_size = 256;
    
    #pragma omp parallel num_threads(NUM_THREADS) proc_bind(spread)
    {
        int tid = omp_get_thread_num();
        
        // 每个线程负责矩阵的一部分
        #pragma omp for
        for (int bi = 0; bi < N; bi += block_size) {
            for (int bj = 0; bj < N; bj += block_size) {
                // 分块计算
                for (int i = bi; i < bi + block_size && i < N; i++) {
                    for (int j = bj; j < bj + block_size && j < N; j++) {
                        double sum = 0.0;
                        for (int k = 0; k < N; k++) {
                            sum += A[i*N + k] * B[k*N + j];
                        }
                        C[i*N + j] = sum;
                    }
                }
            }
        }
    }
}

// 使用嵌套并行和层次亲和性的矩阵乘法
void parallel_matrix_multiply_nested(double *A, double *B, double *C) {
    // 设置允许嵌套并行
    omp_set_nested(1);
    omp_set_max_active_levels(2);
    
    // 外层使用16个线程，内层使用16个线程，总共256个
    int outer_threads = 16;
    int inner_threads = 16;
    
    #pragma omp parallel num_threads(outer_threads) proc_bind(spread)
    {
        int outer_id = omp_get_thread_num();
        int chunk_size = N / outer_threads;
        int start_row = outer_id * chunk_size;
        int end_row = (outer_id == outer_threads - 1) ? N : start_row + chunk_size;
        
        #pragma omp parallel num_threads(inner_threads) proc_bind(close)
        {
            int inner_id = omp_get_thread_num();
            int inner_chunk = (end_row - start_row) / inner_threads;
            int my_start = start_row + inner_id * inner_chunk;
            int my_end = (inner_id == inner_threads - 1) ? end_row : my_start + inner_chunk;
            
            // 每个线程计算自己负责的行
            for (int i = my_start; i < my_end; i++) {
                for (int j = 0; j < N; j++) {
                    double sum = 0.0;
                    for (int k = 0; k < N; k++) {
                        sum += A[i*N + k] * B[k*N + j];
                    }
                    C[i*N + j] = sum;
                }
            }
        }
    }
    
    // 恢复默认设置
    omp_set_nested(0);
}

// 显示部分线程的亲和性信息（完整256个线程会输出太多）
void display_affinity_info() {
    printf("\n===== 线程亲和性信息 (部分线程) =====\n");
    
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        
        #pragma omp master
        {
            printf("总线程数: %d\n", omp_get_num_threads());
            printf("处理器个数: %d\n", omp_get_num_procs());
        }
        
        #pragma omp barrier
        
        // 只显示部分线程信息，避免输出过多
        if (tid % 16 == 0) {
            #pragma omp critical
            {
                char buffer[100];
                omp_capture_affinity(buffer, sizeof(buffer), NULL);
                printf("线程 %d 亲和性: %s\n", tid, buffer);
            }
        }
    }
    
    printf("===========================\n\n");
}

// 验证结果正确性（只检查部分元素以提高速度）
int verify_results(double *C1, double *C2) {
    // 随机抽查1000个元素
    for (int i = 0; i < 1000; i++) {
        int idx = rand() % (N*N);
        if (fabs(C1[idx] - C2[idx]) > 1e-3) {
            return 0; // 不正确
        }
    }
    return 1; // 正确
}

int main() {
    double *A, *B, *C_serial, *C_parallel;
    double start_time, end_time, serial_time;
    
    // 随机数种子初始化
    srand(time(NULL));
    
    // 设置线程数为256
    omp_set_num_threads(NUM_THREADS);
    
    printf("=== 256线程下的矩阵乘法性能比较 (矩阵大小: %d x %d) ===\n", N, N);
    
    // 显示亲和性信息
    display_affinity_info();
    
    // 初始化矩阵
    printf("初始化矩阵中...\n");
    init_matrix(&A, &B, &C_serial, &C_parallel);
    
    // 1. 串行计算（对于大矩阵，只计算部分作为基准）
    printf("执行串行矩阵乘法(部分计算)...\n");
    start_time = get_time();
    // 只计算前500行作为基准，完整计算太慢
    for (int i = 0; i < 500; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C_serial[i*N + j] = sum;
        }
    }
    end_time = get_time();
    serial_time = (end_time - start_time) * (N / 500.0); // 估算完整时间
    printf("串行执行时间(估算): %.16f 秒\n", serial_time);
    
    // 2. 无亲和性并行计算
    printf("\n执行无亲和性并行矩阵乘法(256线程)...\n");
    start_time = get_time();
    parallel_matrix_multiply_no_affinity(A, B, C_parallel);
    end_time = get_time();
    printf("无亲和性并行执行时间: %.16f 秒\n", end_time - start_time);
    printf("相对串行加速比: %.2f 倍\n", serial_time / (end_time - start_time));
    
    // 3. Close亲和性并行计算
    memset(C_parallel, 0, N*N*sizeof(double));
    printf("\n执行close亲和性并行矩阵乘法(256线程)...\n");
    printf("说明: 让线程在最近的核心上执行，优化缓存共享\n");
    start_time = get_time();
    parallel_matrix_multiply_close(A, B, C_parallel);
    end_time = get_time();
    printf("Close亲和性并行执行时间: %.16f 秒\n", end_time - start_time);
    printf("相对串行加速比: %.2f 倍\n", serial_time / (end_time - start_time));
    
    // 16. Spread亲和性并行计算
    memset(C_parallel, 0, N*N*sizeof(double));
    printf("\n执行spread亲和性并行矩阵乘法(256线程)...\n");
    printf("说明: 尽可能将线程分散到不同核心，优化资源利用\n");
    start_time = get_time();
    parallel_matrix_multiply_spread(A, B, C_parallel);
    end_time = get_time();
    printf("Spread亲和性并行执行时间: %.16f 秒\n", end_time - start_time);
    printf("相对串行加速比: %.2f 倍\n", serial_time / (end_time - start_time));
    
    // 5. 分块+亲和性优化计算
    memset(C_parallel, 0, N*N*sizeof(double));
    printf("\n执行分块+亲和性优化矩阵乘法(256线程)...\n");
    printf("说明: 使用缓存友好的分块技术，配合spread亲和性\n");
    start_time = get_time();
    parallel_matrix_multiply_tiled(A, B, C_parallel);
    end_time = get_time();
    printf("分块+亲和性优化执行时间: %.16f 秒\n", end_time - start_time);
    printf("相对串行加速比: %.2f 倍\n", serial_time / (end_time - start_time));
    
    // 6. 嵌套并行+层次亲和性
    memset(C_parallel, 0, N*N*sizeof(double));
    printf("\n执行嵌套并行+层次亲和性矩阵乘法(16x16=256线程)...\n");
    printf("说明: 外层使用spread分散到不同核心，内层使用close聚集相关线程\n");
    start_time = get_time();
    parallel_matrix_multiply_nested(A, B, C_parallel);
    end_time = get_time();
    printf("嵌套并行+层次亲和性执行时间: %.16f 秒\n", end_time - start_time);
    printf("相对串行加速比: %.2f 倍\n", serial_time / (end_time - start_time));
    
    // 总结
    printf("\n==== 256线程下亲和性优势总结 ====\n");
    printf("1. 缓存友好性: close亲和性使相邻线程共享缓存，减少缓存未命中\n");
    printf("2. 资源分布: spread亲和性让线程分散到更多核心，减少单核负载\n");
    printf("3. 分块技术: 通过分块处理增强数据局部性，与亲和性协同工作\n");
    printf("4. 层次并行: 结合不同亲和性策略创建层次化并行结构\n");
    printf("5. 实际应用: 在256线程高并发情况下，亲和性对性能影响更加明显\n\n");
    
    printf("最佳实践:\n");
    printf("• 当线程多于物理核心时(如这里的256线程)，spread通常比close更有效\n");
    printf("• 嵌套并行+层次亲和性结合了两种策略的优点\n");
    printf("• 无论使用什么亲和性策略，合理的数据分块都能带来额外收益\n");
    
    // 释放内存
    free(A);
    free(B);
    free(C_serial);
    free(C_parallel);
    
    return 0;
}

/*
执行串行矩阵乘法(部分计算)...
串行执行时间(估算): 29.4416476599872112 秒

执行无亲和性并行矩阵乘法(256线程)...
无亲和性并行执行时间: 0.6214314650278538 秒
相对串行加速比: 47.38 倍

执行close亲和性并行矩阵乘法(256线程)...
说明: 让线程在最近的核心上执行，优化缓存共享
Close亲和性并行执行时间: 0.6447150679305196 秒
相对串行加速比: 45.67 倍

执行spread亲和性并行矩阵乘法(256线程)...
说明: 尽可能将线程分散到不同核心，优化资源利用
Spread亲和性并行执行时间: 0.6269659539684653 秒
相对串行加速比: 46.96 倍

执行分块+亲和性优化矩阵乘法(256线程)...
说明: 使用缓存友好的分块技术，配合spread亲和性
分块+亲和性优化执行时间: 3.6331031660083681 秒
相对串行加速比: 8.10 倍

执行嵌套并行+层次亲和性矩阵乘法(16x16=256线程)...
说明: 外层使用spread分散到不同核心，内层使用close聚集相关线程
嵌套并行+层次亲和性执行时间: 0.8666297730524093 秒
相对串行加速比: 33.97 倍
*/