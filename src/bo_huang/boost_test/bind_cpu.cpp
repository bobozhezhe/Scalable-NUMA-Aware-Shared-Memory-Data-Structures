#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sched.h>
#include <numa.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ncores = sysconf(_SC_NPROCESSORS_CONF);
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(rank % ncores, &cpu_mask); // 绑定到当前进程rank对应的CPU核心上

    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask) != 0) {
        perror("sched_setaffinity");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int cpu = sched_getcpu();
    int node = numa_node_of_cpu(cpu);
    printf("Process %d is running on CPU %d", rank, cpu);
    std::cout << " on Node " << node << std::endl;


    MPI_Finalize();

    return 0;
}
