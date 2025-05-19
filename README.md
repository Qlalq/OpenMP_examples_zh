# OpenMP官方示例仓库中文改版

官方使用介绍见 [openmp-examples.tex](openmp-examples.tex)  
（LaTeX未编译成功，可通过AI概括内容）

## 环境准备
- GCC 15版本（可通过conda安装）

## 目录

### 原语介绍类
**目标**：理论学习、复现代码、应用实践（对比普通`parallel for`的优势）

1. **affinity**  
   - 基于局部性原理优化  
   - 测试环境：128核CPU  
   - 测试案例：`affinity/affinity_all.c` 中选用16/64/144线程  
   - 结果：与直接使用`parallel for`相比优化效果不明显  
   - 问题：实践不足，仍需探索。 

### 仓库说明类
1. **sources**  
   - 用途：测试代码完整性  
   - 文件说明：  
     - `omp_version.c`：检测OpenMP版本  
     - 问题：`_OPENMP` 宏检测不准确（conda最新OpenMP显示为4.5，对应OpenMP v201511）  
     - 矛盾点：GCC15实际支持5.1的`masked`语法，说明版本号检测存在偏差  
   - 解决方案：  
     - 根据根目录`History.tex`内容测试版本特性  
     - 测试代码见`check_omp_features.c`  
     - 初步结论：GCC15支持OpenMP 6.0语法  