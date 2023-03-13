#include <unordered_set>
#include <vector>
#include <iostream>
#include <mpi.h>
#include <numa.h>

template<typename T>
class numa_unordered_set {
public:
    numa_unordered_set() {
        int num_numa_nodes = numa_max_node() + 1;
        node_sets_.resize(num_numa_nodes);
    }

    // Inserts an element into the unordered_set associated with the nearest NUMA node.
    void insert(const T& value) {
        int node = nearest_numa_node();
        node_sets_[node].insert(value);
    }

    // Searches for a value across all NUMA nodes 
    // and returns true if found, false otherwise.
    bool contains(const T& value) {
        for (auto& node_set : node_sets_) {
            if (node_set.find(value) != node_set.end()) {
                return true;
            }
        }
        return false;
    }

private:
    int nearest_numa_node() const {
        int cpu = sched_getcpu();
        int node = numa_node_of_cpu(cpu);
        if (node < 0) {
            node = 0;
        }
        return node;
    }

    std::vector<std::unordered_set<T>> node_sets_;
};

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int TIMES = 1000000;
    const int NUM_PROCESSES = 4;

    numa_unordered_set<int> a;

    double start_time = MPI_Wtime();
    // Distribute insert operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        a.insert(i);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Insert elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    start_time = MPI_Wtime();
    // Distribute contains operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        bool value = a.contains(i);
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Contains elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
