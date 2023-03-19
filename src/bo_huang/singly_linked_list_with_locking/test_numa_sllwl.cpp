#include "../lib/singly_linked_list_with_locking.h"

#include <unordered_map>
#include <vector>
#include <iostream>
#include <mpi.h>
#include <numa.h>

template<typename T>
class numa_linked_list_locking{
public:
    numa_linked_list_locking() {
        int num_numa_nodes = numa_max_node() + 1;
        node_lists_.resize(num_numa_nodes);
    }

    template<typename ...Args>
    // Inserts an element into the unordered_map associated with the nearest NUMA node.
    void push_back(Args&& ...args) {
        int node = nearest_numa_node();
        node_lists_[node]->push_back(std::forward<Args>(args)...);
    }

    // Searches for a key across all NUMA nodes 
    // and returns the associated value if found
    bool contains(const T key) {
        int node = nearest_numa_node();
        return node_lists_[node]->contains(key);
        // for (auto& node_list : node_lists_) {
        //     return node_list.contains(key);
        //     // if (iter != node_list.end()) {
        //     //     return iter->second;
        //     // }
        // }
        // Throws an out_of_range exception if the key is not found
        // throw std::out_of_range("key not found");
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

    std::vector<singly_linked_list_with_locking<T> *> node_lists_;
    // singly_linked_list_with_locking<T> * node_lists_[];
};


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int TIMES = 1000000;
    const int NUM_PROCESSES = 4;

    numa_linked_list_locking<int> a;

    int num = rank*TIMES;
    double start_time = MPI_Wtime();
    // Distribute emplace operations across all processes
    for (int i = 0; i < TIMES; i += NUM_PROCESSES) {
        num += i;
        a.push_back(num);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    start_time = MPI_Wtime();
    // Distribute get operations across all processes
    for (int i = 0; i < TIMES; i += NUM_PROCESSES) {
        num += i;
        bool value = a.contains(num);
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Getting key elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}

