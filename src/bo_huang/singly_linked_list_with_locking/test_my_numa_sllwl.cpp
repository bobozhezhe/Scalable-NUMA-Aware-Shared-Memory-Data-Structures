#include "../lib/singly_linked_list_with_locking.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>
#include <numa.h>
#include <vector>

using namespace std;

const int NUM_THREADS = 4;
const int NUM_ELEMENTS = 100;

template<typename T>
class numa_linked_list_locking {
public:
    numa_linked_list_locking() {
        int num_numa_nodes = numa_max_node() + 1;
        node_lists_.resize(num_numa_nodes);
        for(int i = 0; i< num_numa_nodes; ++i){
            node_lists_[i] = new singly_linked_list_with_locking<int>;
        }
    }

    // Inserts an element into the list associated with the nearest NUMA node.
    void push_back(const T& value) {
        int node = nearest_numa_node();
        node_lists_[node]->push_back(value);
    }

    // Searches for an element across all NUMA nodes 
    // and returns true if found
    bool contains(const T& value) {
        for (auto& node_list : node_lists_) {
            if (node_list->contains(value)) {
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

    std::vector<singly_linked_list_with_locking<T> *> node_lists_;
};

void test_push_back(numa_linked_list_locking<int>& list) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        list.push_back(i);
    }
}

void test_contains(numa_linked_list_locking<int>& list) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        bool contains = list.contains(i);
    }
}

int main_multi_thread() {
    numa_linked_list_locking<int> list;

    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(test_push_back, std::ref(list));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    threads.clear();
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(test_contains, std::ref(list));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int TIMES = 100;
    const int NUM_PROCESSES = 4;

    numa_linked_list_locking<int> a;

    int num = rank*TIMES;
    double start_time = MPI_Wtime();
    // Distribute emplace operations across all processes
    for (int i = 0; i < TIMES; i += NUM_PROCESSES) {
        num += i;
        std::cout << "No = " << i << ", pushing... " << num << std::endl;
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

