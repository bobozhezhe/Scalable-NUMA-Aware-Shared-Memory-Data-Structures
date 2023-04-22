#include <numa.h>
#include <chrono>
#include <iostream>

#define MEM_LENGTH (1024 * 1024 * 1024)  // 1 GB

void test_numa_memcpy()
{
    // 获取 NUMA 节点数和 CPU 核心数
    int num_nodes = numa_max_node() + 1;
    int num_cores = numa_num_configured_cpus();

    // 在 NUMA 节点 0 上分配两个缓冲区
    void* b1 = numa_alloc_onnode(MEM_LENGTH, 0);
    void* b2 = numa_alloc_onnode(MEM_LENGTH, 0);

    // 测试在同一 NUMA 节点上的内存复制性能
    std::cout << "Copying " << MEM_LENGTH << " bytes within the same NUMA node" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    memcpy(b1, b2, MEM_LENGTH);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " ms\n";

    // 在 NUMA 节点 0 和 1 上分别分配两个缓冲区
    void* b3 = numa_alloc_onnode(MEM_LENGTH, 0);
    void* b4 = numa_alloc_onnode(MEM_LENGTH, 1);

    // 测试在不同 NUMA 节点上的内存复制性能
    std::cout << "Copying " << MEM_LENGTH << " bytes across different NUMA nodes" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    memcpy(b3, b4, MEM_LENGTH);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " ms\n";

    // 释放内存
    numa_free(b1, MEM_LENGTH);
    numa_free(b2, MEM_LENGTH);
    numa_free(b3, MEM_LENGTH);
    numa_free(b4, MEM_LENGTH);
}

int main()
{
    // 初始化 NUMA
    numa_set_localalloc();

    // 测试 NUMA 内存复制性能
    test_numa_memcpy();

    return 0;
}
