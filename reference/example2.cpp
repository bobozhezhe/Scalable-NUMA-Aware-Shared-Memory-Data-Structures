#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/unordered_map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/mpi.hpp>
#include <iostream>
#include <string>

using namespace boost::interprocess;
using namespace boost::mpi;

int main(int argc, char* argv[])
{
    // Initialize MPI
    environment env(argc, argv);
    communicator world;

    // Define the shared memory name
    std::string shm_name = "my_shared_memory";

    // Create or open the shared memory segment
    managed_shared_memory segment(create_only, shm_name.c_str(), 65536);

    // Define an allocator for the unordered_map
    typedef allocator<std::pair<const int, int>, managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the unordered_map
    typedef unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyHashMap;

    // Construct the unordered_map in the shared memory
    MyHashMap* map = segment.construct<MyHashMap>("my_map")(segment.get_segment_manager());

    // Add some entries to the unordered_map
    (*map)[1] = 100;
    (*map)[2] = 200;
    (*map)[3] = 300;

    // Synchronize the shared memory segment
    segment.shmem_barrier::operator()(world);

    // Print the unordered_map entries
    if (world.rank() == 0) {
        std::cout << "Rank 0: " << (*map)[1] << ", " << (*map)[2] << ", " << (*map)[3] << std::endl;
    }
    else if (world.rank() == 1) {
        std::cout << "Rank 1: " << (*map)[1] << ", " << (*map)[2] << ", " << (*map)[3] << std::endl;
    }

    // Destroy the unordered_map and the shared memory segment
    segment.destroy<MyHashMap>("my_map");
    shared_memory_object::remove(shm_name.c_str());

    return 0;
}

