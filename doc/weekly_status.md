# Weekly Report

This is the log file for weekly job and plans.

## week 3 (16 Jan, 2023)

### Progress in week 3

1. Finish the simple test case of shared mem in Boost.
1. Get the simple performance data of the case.
1. Build a NUMA test case, but cannot pass compile.
```
g++ test_numa_memcpy.cpp -o test_numa_memcpy -I/usr/lib -lboost_thread -lpthread -lrt
/usr/bin/ld: /tmp/ccBCsYgY.o: in function `main':
test_numa_memcpy.cpp:(.text+0xd8): undefined reference to `numa_alloc_onnode'
/usr/bin/ld: test_numa_memcpy.cpp:(.text+0x261): undefined reference to `numa_free'
collect2: error: ld returned 1 exit status
make: *** [Makefile:2: test] Error 1
```

### Question in week 3

1. Can we use the Ares env to run NUMA case?
1. Simulating NUMA needs VSphere, any other way?

## week 2 (9 Jan, 2023)

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

## Week 1 (2 Jan, 2023)

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
