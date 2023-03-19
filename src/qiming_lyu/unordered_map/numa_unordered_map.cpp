#include <unordered_map>
#include <vector>
#include <iostream>
#include <numa.h>
#include <omp.h>

template<typename Key, typename T>
class numa_unordered_map {
public:
    numa_unordered_map() {
        int num_numa_nodes = numa_max_node() + 1;
        node_maps_.resize(num_numa_nodes);
    }

    template<typename ...Args>
    // Inserts an element into the unordered_map associated with the nearest NUMA node.
    void emplace(Args &&...args) {
        int node = nearest_numa_node();
        node_maps_[node].emplace(std::forward<Args>(args)...);
    }

    // Searches for a key across all NUMA nodes
    // and returns the associated value if found
    T &get(const Key &key) {
        for (auto &node_map: node_maps_) {
            auto iter = node_map.find(key);
            if (iter != node_map.end()) {
                return iter->second;
            }
        }
        // Throws an out_of_range exception if the key is not found
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

int main(int argc, char **argv) {

    const int TIMES = 1000000;
    numa_unordered_map<int, int> a;

    double start_time = omp_get_wtime();
    for (int i = 0; i < TIMES; i++) {
        a.emplace(i, i);
    }
    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;
    std::cout << "emplace elapsed time: " << elapsed_time << " seconds" << std::endl;

    const int nthreads = 4;
    double thread_times[nthreads];
    omp_set_dynamic(0);
    #pragma omp parallel num_threads(nthreads)
    {
        double t_start_time = omp_get_wtime();
        for (int i = 0; i < TIMES; i++) {
            a.get(i);
        }
        double t_end_time = omp_get_wtime();
        thread_times[omp_get_thread_num()] = t_end_time - t_start_time;
        std::cout << "Thread:" << omp_get_thread_num() << " getting key elapsed time:" << t_end_time - t_start_time << std::endl;
    }

    #pragma omp barrier
    double sum = 0;
    for (int i = 0; i < nthreads; i++) {
        sum += thread_times[i];
    }

    double average = sum / nthreads;
    std::cout << "All threads getting key average time: " << average << " seconds" << std::endl;
    return 0;
}
