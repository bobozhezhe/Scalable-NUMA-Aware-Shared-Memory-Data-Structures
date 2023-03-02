template<typename Key, typename T>
class numa_unordered_map {
  vector<unordered_map> node_maps_;

  numa_unordered_map() {
    int num_numa_nodes = 2;
    node_maps_.resize(num_numa_nodes);
  }

  template<typename ...Args>
  void emplace(Args&& ...args) {
    int node = neareset_numa_node();
    node_maps_[node].emplace(std::forward<Args>(args)...);
  }

  T& Get(Key &key) {
    // Check node 0
    auto iter1 = node_maps_[0].find(key)
    if (iter1 != node_maps_[0].end()) {
      return *iter1;
    }

    // Check node 1
    auto iter1 = node_maps_[1].find(key)
    if (iter1 != node_maps_[1].end()) {
      return *iter1;
    }
  }
}

int main() {
  MPI_Init(...)
  int rank;
  MPI_Comm_rank(..., &rank)

  numa_unordered_map a;

  timer.start()
  if (rank == 0) {
    for(int i = 0; i < 1000; ++i) {
      a.emplace(i, i)
    }
  }
  timer.stop()
  Barrier()

  if (rank != 0) {
    for (int i = 0; i < 1000; ++i) {
      a.Get(i))
    }
  }

  MPI_Finalize()
}
