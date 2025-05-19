# OpenMP官方示例仓库中文改版

## 官方资料
- 官方使用说明文档：[openmp-examples.tex](openmp-examples.tex)  
  （LaTeX未编译成功，可通过AI概括核心内容）

## 环境配置
- **推荐编译器**：GCC 15（可通过conda安装）

## 目录结构

### 核心原语模块
**学习路径**：理论→复现→实践（对比常规`parallel for`的性能差异）

1. **affinity**  
   - 设计原理：利用数据局部性优化线程调度  
   - 测试平台：128核CPU环境  
   - 关键案例：  
     - 文件：`affinity/affinity_all.c`  
     - 线程配置：16/64/144线程测试  
   - 实测结果：相比直接`parallel for`无明显性能提升  
   - 现存问题：需进一步探索应用场景  

**推荐实践顺序**：
1. parallel_execution  
2. data_environment  
3. synchronization  
4. tasking  
5. SIMD  
6. directives  
7. ompt_interface  
8. loop_transformations  
9. memory_model  
10. affinity  
11. devices  

### 仓库工具模块
1. **sources**  
   - 核心功能：验证代码完整性  
   - 重要文件说明：  
     - `omp_version.c`：OpenMP版本检测工具  
     - 已知问题：  
       - `_OPENMP`宏检测异常（conda最新OpenMP显示为4.5，对应规范v201511）  
       - 实际支持情况：GCC15可正常使用5.1规范的`masked`语法  
   - 解决方案：  
     1. 参照根目录`History.tex`测试版本特性  
     2. 使用`check_omp_features.c`进行功能验证  
     3. 最终结论：GCC15实际支持OpenMP 6.0语法规范  