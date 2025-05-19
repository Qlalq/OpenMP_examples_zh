/*
* @@name:	affinity_display.1
* @@type:	C
* @@operation:	run
* @@expect:	success
* @@version:	omp_5.0
* @@env:	OMP_DISPLAY_AFFINITY=TRUE OMP_NUM_THREADS=8
*/
/*
omp_display_affinity(NULL) 是一个主动的、针对当前线程的亲和性显示请求。
OMP_DISPLAY_AFFINITY=TRUE 是一个被动的、针对并行区域中所有线程的亲和性显示机制，它会在运行时认为有必要时（通常是配置首次出现或发生改变时）自动打印信息。
要获得注释中精确的 thread_affinity 值，除了设置 OMP_DISPLAY_AFFINITY=TRUE 和 OMP_NUM_THREADS (或在代码中使用 num_threads())，还必须正确设置 OMP_PROC_BIND 和 OMP_PLACES 环境变量来控制线程如何绑定到处理器资源。
输出的格式可以通过 OMP_DISPLAY_AFFINITY_FORMAT 环境变量进行定制。
*/
#include <stdio.h>
#include <omp.h>

int main(void){                  //MAX threads = 8, single socket system

   //API call-- Displays Affinity of Primary Thread
   omp_display_affinity(NULL);

   // API CALL OUTPUT (default format):
   // team_num= 0, nesting_level= 0, thread_num= 0,
   // thread_affinity= 0,1,2,3,4,5,6,7

   // OMP_DISPLAY_AFFINITY=TRUE, OMP_NUM_THREADS=8
   #pragma omp parallel num_threads(8)
   {
     if(omp_get_thread_num()==0)
        printf("1st Parallel Region -- Affinity Reported \n");

   // DISPLAY OUTPUT (default format) has been sorted:
   // team_num= 0, nesting_level= 1, thread_num= 0, thread_affinity= 0
   // team_num= 0, nesting_level= 1, thread_num= 1, thread_affinity= 1
   // ...
   // team_num= 0, nesting_level= 1, thread_num= 7, thread_affinity= 7

      // doing work here
   }

   #pragma omp parallel num_threads( 8 )
   {
      if(omp_get_thread_num()==0)
         printf("%s%s\n","Same Affinity as in Previous Parallel Region",
                         " -- no Affinity Reported\n");

   // NO AFFINITY OUTPUT:
   //(output in 1st parallel region only for OMP_DISPLAY_AFFINITY=TRUE)

      // doing more work here
   }

   // Report Affinity for 1/2 number of threads
   // export OMP_PROC_BIND=close
   // export OMP_PLACES="cores(8)" 
   #pragma omp parallel num_threads( 8/2 )
   {
     if(omp_get_thread_num()==0)
        printf("Report Affinity for using 1/2 of max threads.\n");

   // DISPLAY OUTPUT (default format) has been sorted:
   // team_num= 0, nesting_level= 1, thread_num= 0, thread_affinity= 0,1
   // team_num= 0, nesting_level= 1, thread_num= 1, thread_affinity= 2,3
   // team_num= 0, nesting_level= 1, thread_num= 2, thread_affinity= 4,5
   // team_num= 0, nesting_level= 1, thread_num= 3, thread_affinity= 6,7

     // do work
   }

   return 0;
}
