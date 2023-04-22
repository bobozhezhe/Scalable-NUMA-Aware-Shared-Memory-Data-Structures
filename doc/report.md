# Report

## Background

## Target

1. STD vs Boost (local)

    Test operation performance with STD library functions and Boost library functions respectively.
    This test is only tested locally.
&nbsp;

1. Boost local vs Boost Shared Memory

    Compare the performance difference between boost library functions in local and shared_memory.
&nbsp;

1. NUMA-sensitive Near vs Far

    Based on shared_memory, test the performance difference between allocating memory on the near node and allocating memory on the far node.
    For example, in the shared memory on the near node, create an unordered_map or singly_linked_list, and then compare the performance of accessing data between from the near node and from the far node.

    * Memory copy should be one way of the testing.
&nbsp;

1. NUMA-sensitive data-structure vs non-NUMA (If don't have time, will not do it.)
    Create a numa-sensitive data structure and compare its performance difference with STD and boost standard library on shared_memory.

    For example, building a numa_unordered_map data structure can ensure that the memory of the near node is always applied for during emplace.

    Compare its performance difference with STD and Boost library functions on shared_memory.

## Experience

## Result
