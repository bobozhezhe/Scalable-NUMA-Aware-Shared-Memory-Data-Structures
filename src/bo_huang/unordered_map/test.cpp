#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numa.h>
#include <mpi.h>

// Custom unordered_map class that uses NUMA-aware allocation
template<typename Key, typename T>
class numa_unordered_map {
  std::vector<std::unordered_map<Key, T>> node_maps_; // One unordered_map per NUMA node

  //int nearest_numa_node() {
  //  return numa_node_of_cpu(sched_getcpu());
  //}

 public:
  numa_unordered_map() {
    // Determine the number of NUMA nodes available and resize the node_maps_ vector accordingly
    int num_numa_nodes = numa_max_node() + 1;
    node_maps_.resize(num_numa_nodes);
  }

  // Insert a new key-value pair into the appropriate unordered_map based on the NUMA node of the current CPU core
  template<typename ...Args>
  void emplace(Args&& ...args) {
    int node = nearest_numa_node();
    node_maps_[node].emplace(std::forward<Args>(args)...);
  }

  // Retrieve the value associated with a given key, searching the unordered_maps in order of NUMA node
  T& get(Key key) {
    for (auto& node_map : node_maps_) {
      auto iter = node_map.find(key);
      if (iter != node_map.end()) {
        return iter->second;
      }
    }
    throw std::out_of_range("key not found");
  }

private:
    // Determine which NUMA node the current CPU core is on
    int nearest_numa_node() const {
        int cpu = sched_getcpu();
        int node = numa_node_of_cpu(cpu);
        if (node < 0) {
            node = 0;
        }
        return node;
    }

};

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  numa_unordered_map<int, int> a;

  // Only the master process inserts elements into the unordered_map
  if (rank == 0) {
    for(int i = 0; i < 1000; ++i) {
      a.emplace(i, i);
    }
  }

  // Wait for all processes to synchronize
  MPI_Barrier(MPI_COMM_WORLD);

  // Each process retrieves elements from the unordered_map
  if (rank != 0) {
    for (int i = 0; i < 1000; ++i) {
      int value = a.get(i);
    }
  }

  MPI_Finalize();
  return 0;
}
