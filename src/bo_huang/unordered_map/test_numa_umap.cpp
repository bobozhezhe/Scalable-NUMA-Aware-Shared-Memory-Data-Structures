#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>
#include <mpi.h>
#include <numa.h>

template<typename Key, typename T>
class numa_unordered_map {
public:
    numa_unordered_map() {
        int num_numa_nodes = numa_max_node() + 1;
        node_maps_.resize(num_numa_nodes);
    }

    template<typename ...Args>
    // nserts an element into the unordered_map associated with the nearest NUMA node.
    void emplace(Args&& ...args) {
        int node = nearest_numa_node();
        node_maps_[node].emplace(std::forward<Args>(args)...);
    }

    // searches for a key across all NUMA nodes 
    // and returns the associated value if found
    T& get(const Key& key) {
        for (auto& node_map : node_maps_) {
            auto iter = node_map.find(key);
            if (iter != node_map.end()) {
                return iter->second;
            }
        }
        // hrows an out_of_range exception if the key is not found
        throw std::out_of_range("key not found");
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

    std::vector<std::unordered_map<Key, T>> node_maps_;
};

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int TIMES = 1000000;

    numa_unordered_map<int, int> a;

    if (rank == 0) {
        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < TIMES; ++i) {
            a.emplace(i, i);
        }
        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        std::cout << "Process " << rank << " emplace time: " << elapsed_time.count() << " seconds" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank != 0) {
        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < TIMES; ++i) {
            int value = a.get(i);
        }
        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        std::cout << "Process " << rank << " get time: " << elapsed_time.count() << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}

