# Weekly Report

This is the log file for weekly job and plans.

---
## Week 9 (30 Mar, 2023)

1. Finished all the test cases. singly_linked_list_with_locking changes to no locking.

## Week 8 (23 Mar, 2023)

1. Finish most of the test cases, except the singly_linked_list_with_locking.

## Week 7 (16 Mar, 2023)

1. Fix the Boost test case using unorder map.(Now we have a full case for one data-structure.)
1. Finish Writing NUMA and Boost test cases for unorder_set.
1. Trying to write NUMA and Boost test cases for singly_linked_list_with_locking.
1. Change to CMake.

### Plan for week 8

1. Fix the cases for unorder_set and singly_linked_list_with_locking.
1. Finish the mid-report.
1. Finish more test case for other data-structure.

## Week 6 (9 Mar, 2023)

1. Fix the NUMA test case using unorder map.
1. Write the Boost test case using unorder map.

## Week 5 (2 Mar, 2023)

1. Write a NUMA test case using unorder map.
1. Make evaluation plan.

### Plan for week 6

## Week 4 (23 Feb, 2023)

Skipped, for the too many assignments and midterm exam.

## Week 3 (16 Feb, 2023)

### Progress in week 3

1. Finish the simple test case of shared mem in Boost.
1. Get the simple performance data of the case.
1. Build a NUMA test case, but cannot pass compile.

### Plan for week 3

1. Build a OpenMPI test case.
1. Each of us builds a test case based on a data structure, and tests the performance on shared memory or NUMA nodes.
1. Build test cases in a way that can port to a machine with more than 1 NUMA node
1. Evaluation plan. What were your test cases going to show and how?

### Question in week 3

1. Can we use the Ares env to run NUMA case?
1. Simulating NUMA needs VSphere, any other way?
1. Do you know how to solve the compile error? Log info is as following:

```log
g++ test_numa_memcpy.cpp -o test_numa_memcpy -I/usr/lib -lboost_thread -lpthread -lrt
/usr/bin/ld: /tmp/ccBCsYgY.o: in function `main':
test_numa_memcpy.cpp:(.text+0xd8): undefined reference to `numa_alloc_onnode'
/usr/bin/ld: test_numa_memcpy.cpp:(.text+0x261): undefined reference to `numa_free'
collect2: error: ld returned 1 exit status
make: *** [Makefile:2: test] Error 1
```

## week 2 (9 Feb, 2023)

### Progress in week 2

1. Finish building the env of MPI, running a sample test program.
1. Finish building the env of Boost, running an interprocess test program.
1. Build a simple NUMA env in software, not tested yet.

### Question in week 2

1. Can we use some third-party libraries?
1. How can we run the sample program given in your email.
1. Is there any constraints in NUMA env? (latency? network?)

### Plan for week 3

1. Build shared mem test cases.
1. Try to get the performance data of these test cases.
1. Try to run a simple case in NUMA env.

### Reference

1. <https://github.com/scs-lab/labstor/blob/master/include/labstor/userspace/util/timer.h>

## Week 1 (2 Feb, 2023)

### Progress in week 1

1. Finish Project Proposal.
2. Learn and understand the common conception of NUMA.

### Question in week 1

1. How to set up the environment: ubuntu + clion + gcc +boost?
2. What is the tool to evaluate performance? PMDK, Intel Memory Latency Checker, other?
3. Recommended tools to evaluate shared memory
4. What's the kind of test case?
5. What's test env? Anything we need to install in our dev env?

### Plan for week 2

1. Build up dev env. (ubuntu + clion + gcc +boost)
2. Write a simple test case for mem test.
3. Prepare test cases for shared mem. (Random data + multi-way access + shared mem)
4. Set up and run the test software to show the perm of Boost.
5. Create several test cases with random numbers of each data structure, in different ways of memory allocation and compare their performances.
