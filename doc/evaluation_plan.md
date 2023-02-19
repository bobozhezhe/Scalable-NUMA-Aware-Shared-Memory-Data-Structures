# 1. Evaluation Plan

---

## 1.1. Build benchmarks for each data structures

### 1.1.1. Build an applicable algorithm

1. For a suitable algorithm case, it should fully use the shared memory complicatedly. It is best if the operation of the algorithm can separate the read operation and the write operation according to different requirements.

1. Test and record. Use this case or multiple cases, test on different NUMA nodes, and record performance differences. It should be possible to deploy to all NUMA nodes simultaneously at runtime. Deploy script should support to record the test result automaticly.

### 1.1.2. Optimize the data structure to NUMA-aware data structure and test

1. Build NUMA-aware data structure
1. Test and performance recorded on different NUMA nodes.
1. Test the following optimizations
    1. Read Nearness.
    2. Write Nearness.

## 1.2. Reference

### 1.2.1. Data structure

1. singly linked list (with locking)
2. parallel queue (with locking, very similar to singly linked list)
3. unordered_map (array-based)
4. unordered_set (array_based)
5. unordered_map (list-based, using the list in (1))
6. unordered_set (list-based, using the list in (1))
7. lock-free queue (if there's time, but probably not)
8. lock-free singly linked list (if there's time, but probably not)

### 1.2.2. Optimizations

The main NUMA Optimizations (applies to each of the above data structures) are as follows:

1. Read Nearness: Let's say your machine has 4 NUMA nodes. The NUMA-aware unordered_map would be a wrapper around 4 regular unordered_maps (1 per NUMA node). During an emplace, the object would be created 4 times. During a read, the object would be read from the nearest NUMA node. Writes would be penalized for the sake of read optimization.
2. Write Nearness: A different NUMA-aware unordered_map may focusing on improving the performance of an "emplace". This NUMA-aware unordered_map would be a wrapper around 4 regular unordered_maps (1 per NUMA node). During an emplace, the object would be placed in the unordered_map on the nearest NUMA node. Reads would have to scan each NUMA node to guarantee finding the object. Writes would be optimized. Reads may be penalized depending on the workload.
