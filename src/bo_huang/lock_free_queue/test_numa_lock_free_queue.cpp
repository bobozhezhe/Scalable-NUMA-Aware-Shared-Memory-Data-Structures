#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <numa.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    constexpr int NUM_ELEMENTS = 100;
    constexpr int NUM_PROCESSES = 4;

    // Get the number of NUMA nodes on the system
    int num_nodes = numa_max_node() + 1;
    std::cout << "Number of Node: " << num_nodes << std::endl;

    // Create a lock-free queue associated with each NUMA node
    std::vector<boost::lockfree::queue<int, boost::lockfree::capacity<NUM_ELEMENTS>>> queues(num_nodes);

    // Measure the time it takes to push elements onto the queues
    double start_time = MPI_Wtime();
    for (int i = rank; i < NUM_ELEMENTS; i += NUM_PROCESSES) {
        int node = numa_node_of_cpu(sched_getcpu());
        node = node == -1 ? 0 : node;
        std::cout << "Pushing... Node: " << node << ", i = " << i << std::endl;
        queues[node].push(i);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Pushing elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    std::cout << "Finish... pushing" << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);

    // Measure the time it takes to pop elements from the queues
    start_time = MPI_Wtime();
    for (int i = rank; i < NUM_ELEMENTS; i += NUM_PROCESSES) {
        int node = numa_node_of_cpu(sched_getcpu());
        node = node == -1 ? 0 : node;
        int val;
        std::cout << "Popping... Node: " << node << ", i = " << i << std::endl;
        // int tmp = queues[node].pop(val);
        // std::cout << tmp << std::endl;
        while (queues[node].pop(val)) {} // Keep trying to pop until successful
    }
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Popping elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}


// #include <unordered_map>
// #include <vector>
// #include <iostream>
// #include <mpi.h>
// #include <numa.h>
// #include "../lib/lock_free_queue.h"

// template<typename T>
// class numa_free_queue {
// public:
//     numa_free_queue () {
//         int num_numa_nodes = numa_max_node() + 1;
//         node_queues_.resize(num_numa_nodes);
//     }

//     template<typename ...Args>
//     // Inserts an element into the unordered_map associated with the nearest NUMA node.
//     void enqueue(Args&& ...args) {
//         int node = nearest_numa_node();
//         node_queues_[node]->enqueue(std::forward<Args>(args)...);
//     }

//     // Searches for a key across all NUMA nodes 
//     // and returns the associated value if found
//     bool dequeue(T& result) {
//         int node = nearest_numa_node();
//         return node_queues_[node]->dequeue(result);
//     }
//     // bool dequeue(T& result) {
//         //for (auto& node_queue : node_queues_) {
//         //    auto iter = node_queue.find(key);
//         //    if (iter != node_queue.end()) {
//         //        return iter->second;
//         //    }
//         //}
//         // Throws an out_of_range exception if the key is not found
//         // throw std::out_of_range("key not found");
//     // }

// private:
//     int nearest_numa_node() const {
//         int cpu = sched_getcpu();
//         int node = numa_node_of_cpu(cpu);
//         if (node < 0) {
//             node = 0;
//         }
//         return node;
//     }

//     std::vector<lock_free_queue<T> *> node_queues_;
// };

// int main(int argc, char** argv) {
//     MPI_Init(&argc, &argv);
//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     const int TIMES = 1000000;
//     const int NUM_PROCESSES = 4;

//     numa_free_queue<int> a;

//     double start_time = MPI_Wtime();
//     // Distribute emplace operations across all processes
//     for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
//         a.enqueue(i);
//     }
//     double end_time = MPI_Wtime();
//     double elapsed_time = end_time - start_time;
//     double max_elapsed_time;
//     MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
//     if (rank == 0) {
//         std::cout << "Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
//     }

//     MPI_Barrier(MPI_COMM_WORLD);

//     start_time = MPI_Wtime();
//     // Distribute get operations across all processes
//     for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
//         int value = 0;
//         int& res = value;
//         bool state = a.dequeue(res);
//     }
//     end_time = MPI_Wtime();

//     elapsed_time = end_time - start_time;
//     MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

//     if (rank == 0) {
//         std::cout << "Getting key elapsed time: " << max_elapsed_time << " seconds" << std::endl;
//     }

//     MPI_Finalize();
//     return 0;
// }

