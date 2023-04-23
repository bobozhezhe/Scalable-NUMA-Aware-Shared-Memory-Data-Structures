#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sched.h>
#include <numa.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ncores = sysconf(_SC_NPROCESSORS_CONF);
    if (rank == 0)
        std::cout << "CPU total num = " << ncores << std::endl;
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(rank % ncores, &cpu_mask); // 绑定到当前进程rank对应的CPU核心上

    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask) != 0)
    {
        perror("sched_setaffinity");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // get the cpu id and node
    int cpu = sched_getcpu();
    int node = numa_node_of_cpu(cpu);
    printf("Process %d is running on CPU %d", rank, cpu);
    std::cout << " on Node " << node << std::endl;

    MPI_Finalize();

    return 0;
}

// numa_node_to_cpus(), numa_node_of_cpu(), numa_bind(),
// numa_run_on_node(), numa_run_on_node_mask() and numa_get_run_node_mask().
// These functions deal with the CPUs associated with numa nodes.

int check_numa_cpu()
{
    int num_nodes = numa_max_node() + 1;
    std::cout << "Max nodes = " << num_nodes << std::endl;

    for (int i = 0; i < num_nodes; ++i)
    {
        long cpus = numa_node_size(i, NULL);
        printf("Node %d has %ld CPUs\n", i, cpus);
        long free_mem;
        long mem = numa_node_size(i, &free_mem);
        printf("Node %d has %ld bytes of memory (%lu bytes free)\n", i, mem, free_mem);
    }
    return 0;
}

#include <stdio.h>
#include <numaif.h>

// int test() {
//     int num_nodes = numa_max_node() + 1;
//     for (int i = 0; i < num_nodes; ++i) {
//         struct bitmask *cpus = numa_node_cpus(i);
//         int num_cpus = numa_bitmask_weight(cpus);
//         printf("Node %d has %d CPUs\n", i, num_cpus);
//         numa_bitmask_free(cpus);
//     }
//     return 0;
// }
