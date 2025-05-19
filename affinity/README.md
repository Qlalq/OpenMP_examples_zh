# OpenMP 亲和性（Affinity）相关文档概述

此文件夹包含四个 LaTeX 文件，详细介绍了 OpenMP 的线程和任务亲和性功能。这些文件从不同的角度讲解了如何控制、显示和查询 OpenMP 线程及任务在硬件上的绑定。

## 文件关系和内容概述
官方示例代码在`sources/`目录下。
这四个文件之间的逻辑关系和主要内容如下：

1.  **`affinity/affinity.tex（核心）`**
    *   **核心概念**: 讲解 OpenMP 线程亲和性控制。
    *   **内容**: 重点介绍了 `proc_bind` 子句及其不同的策略（`spread`、`close`、`primary`）。解释了如何使用 `OMP_PLACES` 环境变量来定义"位置"（places）以及这些位置如何影响线程的绑定。文件通过示例展示了不同策略在特定机器架构下的线程绑定结果。
    *   **官方代码**: `affinity.1.c`~`affinity.5.c`

2.  **`affinity/affinity_display.tex`**
    *   **核心概念**: 讲解如何**显示**线程亲和性信息。
    *   **内容**: 介绍了控制亲和性信息自动显示的 `OMP_DISPLAY_AFFINITY` 环境变量和自定义输出格式的 `OMP_AFFINITY_FORMAT` 环境变量。同时，详细说明了用于在代码中按需显示亲和性的 API 函数，包括 `omp_display_affinity`、`omp_get_affinity_format`、`omp_set_affinity_format` 和 `omp_capture_affinity`。
    *   **官方代码**: `affinity_display1.c`, `affinity_display.2.c`介绍`omp_display_affinity`；
    `affinity_display.3.c`介绍`omp_get_affinity_format`、`omp_set_affinity_format` 和 `omp_capture_affinity`

3.  **`affinity/affinity_query.tex`**
    *   **核心概念**: 侧重于**查询**亲和性相关的属性。
    *   **内容**: 介绍了允许程序在运行时获取关于 Places（位置）和线程绑定信息的 API 函数，例如 `omp_get_num_places`（获取可用 Places 数量）、`omp_get_place_num`（获取当前线程所属 Place 的编号）和 `omp_get_place_num_procs`（获取当前 Place 中的处理器数量）。
    *   **官方代码**: `affinity_query.1.c`

4.  **`affinity/task_affinity.tex`**
    *   **核心概念**: 专门讨论**任务亲和性**（Task Affinity）。
    *   **内容**: 介绍了 `task` 构造的 `affinity` 子句。这个子句提供一个提示给运行时系统，建议将任务调度到靠近任务中指定变量（特别是数组或指针引用的数据）物理存储位置的处理器上执行。文件通过示例区分了任务亲和性与并行区域线程绑定的不同。
    *   **官方代码**: `affinity.6.c`

## 补充说明
- **综合演示**: `affinity_all.c` 为集成中文说明的综合示例代码，包含完整的中文注释和多种策略的对比实现
- **代码位置**: 所有原始官方示例代码均位于`sources/`子目录中

## 总结

这四个文件共同构成了 OpenMP 亲和性功能的文档体系：

*   `affinity.tex` - **如何控制**并行区域的线程绑定
*   `affinity_display.tex` 和 `affinity_query.tex` - **如何观察和获取**线程绑定信息
*   `task_affinity.tex` - **如何控制**独立任务的亲和性

通过阅读这些文件，结合`sources/`目录下的官方示例和`affinity_all.c`中文综合示例，可以全面了解 OpenMP 提供的各种亲和性控制和查询机制。