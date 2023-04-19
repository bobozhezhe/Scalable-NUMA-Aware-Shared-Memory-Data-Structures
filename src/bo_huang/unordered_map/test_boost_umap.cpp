#include <boost/mpi.hpp>
#include <boost/unordered_map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <memory>

using namespace boost::interprocess;
using namespace boost::mpi;

int main(int argc, char** argv) {
    const int TIMES = 1000000;
    const int MEM_LENGTH = 256 * (1 << 20);  // 256MB
    const int MAP_LENGTH = 1024;

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment
    std::unique_ptr<boost::interprocess::managed_shared_memory> segment;

    if (rank == 0) {
      shared_memory_object::remove(shm_name.c_str());
      segment = std::make_unique<boost::interprocess::managed_shared_memory>(
        open_or_create, shm_name.c_str(), MEM_LENGTH);
    }
    comm.barrier();
    if (rank != 0) {
      segment = std::make_unique<boost::interprocess::managed_shared_memory>(
        open_only, shm_name.c_str());
    }
    std::cout << "Rank: " << rank << " segment: " << segment.get() << std::endl;

    // Define an allocator for the unordered_map
    typedef boost::interprocess::allocator<std::pair<const int, int>, managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the unordered_map
    typedef boost::unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyHashMap;

    // Construct the unordered_map in the shared memory
    MyHashMap *map = nullptr;
    if (rank == 0) {
      map = segment->construct<MyHashMap>("my_map")(
        segment->get_segment_manager());
    }
    comm.barrier();
    if (rank != 0) {
      map = segment->find<MyHashMap>("my_map").first;
    }
    assert(map != nullptr);
    std::cout << "Rank: " << rank << " map: " << map << std::endl;

    double start_time = MPI_Wtime();
    // Distribute emplace on a single process
    if (rank == 0) {
      for (int i = 0; i < TIMES; ++i) {
        map->emplace(i % MAP_LENGTH, i % MAP_LENGTH);
      }
      std::cout << "Finished emplace" << std::endl;
    }
    comm.barrier();

    // Elapsed time of emplace
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;
    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());
    std::cout << "Rank: " << rank << " Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;

    // Distribute get operations across all processes
    comm.barrier();
    start_time = MPI_Wtime();
    for (int i = 0; i < TIMES; ++i) {
        // std::cout << "Get: " << i << std::endl;
        auto iter = map->find(i % MAP_LENGTH);
        assert(iter != map->end());
        std::pair<int, int> p = *iter;
        int val = p.second;
        assert(val == (i % MAP_LENGTH));
        // int value = (*map)[i % MAP_LENGTH];
    }
    end_time = MPI_Wtime();

    // Get elapsed time of GET
    elapsed_time = end_time - start_time;
    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());
    std::cout << "Rank: " << rank << " Get elapsed time: " << max_elapsed_time << " seconds" << std::endl;


    comm.barrier();
    if (comm.rank() == 0) {
        // Destroy the unordered_map and the shared memory segment
        std::cout << "Rank 0: destroyign memory.";
        segment->destroy<MyHashMap>("my_map");
        shared_memory_object::remove(shm_name.c_str());
        std::cout << "Rank 0: destroyed memory.";
    }

    return 0;
}
