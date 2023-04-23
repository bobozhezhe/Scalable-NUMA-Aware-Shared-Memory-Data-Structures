
#include <iostream>
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <mpi.h>
#include <numa.h>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <unordered_map>

using namespace boost::interprocess;

typedef std::pair<const int, std::string> map_value_t;
typedef allocator<map_value_t, managed_shared_memory::segment_manager> map_allocator_t;
typedef map<int, std::string, std::less<int>, map_allocator_t> unordered_map;


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *shared_memory_name = "unordered_map_shared_memory";
    const char *shared_unordered_map = "my_shared_unordered_map";
    const std::size_t shared_memory_size = 256 * (1 << 20);
    std::string count = argv[2];
    int data_count = std::stoi(count);
    int cpu = sched_getcpu();
    int node = numa_node_of_cpu(cpu);

    shared_memory_object::remove(shared_memory_name);
    MPI_Barrier(MPI_COMM_WORLD);

    unordered_map *shared_map = nullptr;
    managed_shared_memory *segment = nullptr;
    std::unordered_map<int, std::string> stl_map;
    // Rank 0 creates the shared memory and initializes the map
    if (rank == 0) {
        std::cout << "rank " << rank << " on node " << node << " creating shared memory and initializing map..." << std::endl;
        double start_time = MPI_Wtime();
        segment = new managed_shared_memory(create_only, shared_memory_name, shared_memory_size);
        map_allocator_t map_allocator(segment->get_segment_manager());
        shared_map = segment->construct<unordered_map>(shared_unordered_map)(std::less<int>(), map_allocator);
        for (int i = 0; i < data_count; ++i) {
            (*shared_map)[i] = std::to_string(i);
        }

        double end_time = MPI_Wtime();
        std::cout << "rank " << rank << " on node " << node << " data count " << data_count << " emplace shared_map elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;

        start_time = MPI_Wtime();
        for (int i = 0; i < data_count; ++i) {
            stl_map[i] = std::to_string(i);
        }
        end_time = MPI_Wtime();
        std::cout << "rank " << rank << " on node " << node << " data count " << data_count << " emplace stl_set elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;

        start_time = MPI_Wtime();
        for (auto it = stl_map.begin(); it != stl_map.end(); ++it) {
            auto iterator = *it;
        }
        end_time = MPI_Wtime();
        std::cout << "rank " << rank << " on node " << node << " read stl_set elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank != 0) {
        // other processes open the shared memory and the map
        std::cout << "rank " << rank << " on node " << node << " opening shared memory and map..." << std::endl;
        segment = new managed_shared_memory(open_read_only, shared_memory_name);
        shared_map = segment->find<unordered_map>(shared_unordered_map).first;
    }

    // All processes traverse the map and measure the time
    double start_time = MPI_Wtime();
    for (auto it = shared_map->begin(); it != shared_map->end(); ++it) {
        auto &first = it->first;
        auto &second = it->second;
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    std::cout << "rank " << rank << " on node " << node << " read shared_map elapsed time: " << elapsed_time * 1000 << " millisecond" << std::endl;

    // Rank 0 prints the elapsed time
    double total_elapsed_time;
    MPI_Reduce(&elapsed_time, &total_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Total read shared_map elapsed time: " << total_elapsed_time * 1000 << " millisecond" << std::endl;

        // Destroy the unordered_map and the shared memory segment
        segment->destroy<unordered_map>(shared_unordered_map);
        shared_memory_object::remove(shared_memory_name);
    }

    MPI_Finalize();
    return 0;
}
