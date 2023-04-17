
#include <iostream>
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <mpi.h>
#include <boost/interprocess/sync/named_mutex.hpp>

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
    const std::size_t shared_memory_size = 1024*1024*10;

    shared_memory_object::remove(shared_memory_name);

    unordered_map *shared_map = nullptr;
    managed_shared_memory *segment = nullptr;
    // Rank 0 creates the shared memory and initializes the map
    if (rank == 0) {
        std::cout << "rank " << rank << " creating shared memory and initializing map..." << std::endl;
        double start_time = MPI_Wtime();
        segment = new managed_shared_memory(create_only, shared_memory_name, shared_memory_size);
        map_allocator_t map_allocator(segment->get_segment_manager());
        shared_map = segment->construct<unordered_map>(shared_unordered_map)(std::less<int>(), map_allocator);

        for (int i = 0; i < 100000; ++i) {
            (*shared_map)[i] = std::to_string(i);
        }

        double end_time = MPI_Wtime();
        std::cout << "rank " << rank << " emplace elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "rank " << rank << " sleep..." << std::endl;
    // Create a named mutex to ensure mutual exclusion
    named_mutex mutex(open_or_create, "my_named_mutex");
    mutex.unlock();
    if (rank != 0) {
        // other processes open the shared memory and the map
        while (segment == nullptr) {
            try {
                std::cout << "rank " << rank << " opening shared memory and map..." << std::endl;
                mutex.lock();
                segment = new managed_shared_memory(open_only, shared_memory_name);
                mutex.unlock();
            }
            catch (const std::exception& e) {
                std::cerr <<"rank " << rank << " error opening shared memory: " << e.what() << std::endl;
                mutex.unlock();
                usleep(500000);
//                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        std::cout << "rank " << rank << " opening shared memory and map... 1" << std::endl;
        shared_map = segment->find<unordered_map>(shared_unordered_map).first;
        std::cout << "rank " << rank << " opening shared memory and map... 2" << std::endl;
    }

    // All processes traverse the map and measure the time
    double start_time = MPI_Wtime();
    for (auto it = shared_map->begin(); it != shared_map->end(); ++it) {
        auto &first = it->first;
        auto &second = it->second;
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    std::cout << "rank:" << rank << " elapsed time: " << elapsed_time * 1000 << " millisecond" << std::endl;

    // Rank 0 prints the elapsed time
    double total_elapsed_time;
    MPI_Reduce(&elapsed_time, &total_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Total elapsed time: " << total_elapsed_time * 1000 << " millisecond" << std::endl;

        // Destroy the unordered_map and the shared memory segment
        std::cout << "rank " << rank << " try remove memory" << std::endl;
        segment->destroy<unordered_map>(shared_unordered_map);
        shared_memory_object::remove(shared_memory_name);
        std::cout << "rank " << rank << " removed memory" << std::endl;
    }

    std::cout << "rank " << rank << " try finalize" << std::endl;
    MPI_Finalize();
    std::cout << "rank " << rank << " finalized" << std::endl;
    return 0;
}
