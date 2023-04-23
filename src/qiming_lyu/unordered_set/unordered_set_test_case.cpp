
#include <iostream>
#include <string>
#include <boost/unordered_set.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <mpi.h>
#include <numa.h>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <unordered_set>

using namespace boost::interprocess;

typedef boost::interprocess::allocator<int, managed_shared_memory::segment_manager> ShmemAllocator;
// Define the unordered_set
typedef boost::unordered_set<int, std::hash<int>, std::equal_to<>, ShmemAllocator> MyHashSet;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *shared_memory_name = "unordered_set_shared_memory";
    const char *shared_unordered_set = "my_shared_unordered_set";
    const std::size_t shared_memory_size = 256 * (1 << 20);
    std::string count = argv[2];
    int data_count = std::stoi(count);
    int cpu = sched_getcpu();
    int node = numa_node_of_cpu(cpu);

    shared_memory_object::remove(shared_memory_name);
    MPI_Barrier(MPI_COMM_WORLD);

    MyHashSet *shared_set = nullptr;
    managed_shared_memory *segment = nullptr;
    std::unordered_set<int> stl_set;
    // Rank 0 creates the shared memory and initializes the set
    if (rank == 0) {
        std::cout << "rank " << rank << " on node " << node << " creating shared memory and initializing set..." << std::endl;
        double start_time = MPI_Wtime();
        segment = new managed_shared_memory(create_only, shared_memory_name, shared_memory_size);
        shared_set = segment->construct<MyHashSet>(shared_unordered_set)(segment->get_segment_manager());
        for (int i = 0; i < data_count; ++i) {
            shared_set->insert(i);
        }
        double end_time = MPI_Wtime();
        std::cout << "rank " << rank << " on node " << node << " data count " << data_count << " emplace shared_set elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;

        start_time = MPI_Wtime();
        for (int i = 0; i < data_count; ++i) {
            stl_set.insert(i);
        }
        end_time = MPI_Wtime();
        std::cout << "rank " << rank << " on node " << node << " data count " << data_count << " emplace stl_set elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;

        start_time = MPI_Wtime();
        for (auto it = stl_set.begin(); it != stl_set.end(); ++it) {
            auto iterator = *it;
        }
        end_time = MPI_Wtime();
        std::cout << "rank " << rank << " on node " << node << " read stl_set elapsed time: " << (end_time - start_time)*1000 << " millisecond" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank != 0) {
        // other processes open the shared memory and the set
        std::cout << "rank " << rank << " on node " << node << " opening shared memory and set..." << std::endl;
        segment = new managed_shared_memory(open_read_only, shared_memory_name);
        shared_set = segment->find<MyHashSet>(shared_unordered_set).first;
    }

    // All processes traverse the set and measure the time
    double start_time = MPI_Wtime();
    for (auto it = shared_set->begin(); it != shared_set->end(); ++it) {
        auto iterator = *it;
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    std::cout << "rank " << rank << " on node " << node << " read shared_set elapsed time: " << elapsed_time * 1000 << " millisecond" << std::endl;

    // Rank 0 prints the elapsed time
    double total_elapsed_time;
    MPI_Reduce(&elapsed_time, &total_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Total shared memory read elapsed time: " << total_elapsed_time * 1000 << " millisecond" << std::endl;

        // Destroy the unordere_set and the shared memory segment
        segment->destroy<MyHashSet>(shared_unordered_set);
        shared_memory_object::remove(shared_memory_name);
    }

    MPI_Finalize();
    return 0;
}
