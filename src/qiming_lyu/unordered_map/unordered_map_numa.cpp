#include <unordered_map>
#include <vector>
#include <iostream>
#include <numa.h>
#include <omp.h>
#include <unistd.h>

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

    int size(int node) {
        return node_maps_[node].size();
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
    std::string count = argv[2];
    int TIMES = std::stoi(count);
    //get num of nodes
    const int num_numa_nodes = numa_max_node() + 1;
    const int step = num_numa_nodes;
    numa_unordered_map<int, int> *map = new numa_unordered_map<int, int>;
    double thread_read_local_times[num_numa_nodes];
    double thread_read_other_times[num_numa_nodes];
    double thread_write_times[num_numa_nodes];

    //create the same number of threads as there are nodes
#pragma omp parallel num_threads(num_numa_nodes)
    {
        //each thread inserts data at the local node
        int thread_num = omp_get_thread_num();
        int node = thread_num % num_numa_nodes;
        numa_run_on_node(node);

        if (thread_num == 0) {
            double start_time = omp_get_wtime();
            for (int i = thread_num; i < TIMES; i++) {
                map->emplace(i, i);
            }
            double end_time = omp_get_wtime();
            double elapsed_time = end_time - start_time;
            thread_write_times[omp_get_thread_num()] = elapsed_time;
            std::cout << "Thread " << thread_num << " on node " << node << " data count " << TIMES << " emplace elapsed time: " << elapsed_time << " seconds" << std::endl;
        }

#pragma omp barrier
        std::cout << "map count " << map->size(node) << std::endl;

        //first, reads the data from the local node
        double t_start_time = omp_get_wtime();
        int start_index = thread_num;
        for (int i = start_index; i < TIMES; i += step) {
            map->get(i);
        }
        double t_end_time = omp_get_wtime();
        double t_elapsed_time = t_end_time - t_start_time;
        thread_read_local_times[omp_get_thread_num()] = t_elapsed_time;
        std::cout << "Thread " << thread_num << " on node " << node << " read node 0" << " get elapsed time: "
                  << t_elapsed_time << " seconds" << std::endl;
    }
#pragma omp barrier
    // calculation execution time
    int thread_num = omp_get_thread_num();
    if (thread_num == 0) {
        double sum = 0;
        for (int i = 0; i < num_numa_nodes; i++) {
            sum += thread_write_times[i];
        }
        double average = sum / num_numa_nodes;
        std::cout << "All threads write key average time: " << average << " seconds" << std::endl;

        sum = 0;
        for (int i = 0; i < num_numa_nodes; i++) {
            sum += thread_read_local_times[i];
        }
        average = sum / num_numa_nodes;
        std::cout << "All threads get key on local node average time: " << average << " seconds" << std::endl;

        sum = 0;
        for (int i = 0; i < num_numa_nodes; i++) {
            sum += thread_read_other_times[i];
        }
        average = sum / num_numa_nodes;
        std::cout << "All threads get key on other node average time: " << average << " seconds" << std::endl;
    }

    return 0;
}
