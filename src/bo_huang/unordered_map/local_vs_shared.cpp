
#include <iostream>
#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <chrono>
#include "../lib/numa_experience.h"

const int EMPLACE_NUM = 100;
const int AT_NUM = 100;

void test_boost_unordered_map()
{

    // 初始化unordered_map
    boost::unordered_map<int, int> my_map;

    std::cout << "boost::unordered_map emplace ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < EMPLACE_NUM; i++) {
            my_map.emplace(i, i);
        });

    std::cout << "boost::unordered_map at ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < AT_NUM; i++) {
            my_map.at(i % EMPLACE_NUM);
        });
}

void test_std_unordered_map()
{
    // 初始化unordered_map
    std::unordered_map<int, int> my_map;
    std::cout << "std::unordered_map emplace  ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < EMPLACE_NUM; i++) {
            my_map.emplace(i, i);
        });

    std::cout << "std::unordered_map at  ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < AT_NUM; i++) {
            my_map.at(i % EMPLACE_NUM);
        });
}

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

// using namespace boost::interprocess;
namespace bip = boost::interprocess;

void test_shared_memory_boost_unordered_map()
{
    typedef bip::allocator<std::pair<const int, int>, bip::managed_shared_memory::segment_manager> ShmemAllocator;
    typedef boost::unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyMap;

    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 64* (1 << 10);

    // Initialize MPI
    boost::mpi::environment env();
    boost::mpi::communicator comm;

    std::string shm_name = "my_shared_memory";
    // Remove shared memory if it already exists
    bip::shared_memory_object::remove(shm_name.c_str());

    // Create shared memory
    // managed_shared_memory segment(create_only, "MySharedMemory", 65536);
    std::unique_ptr<boost::interprocess::managed_shared_memory> segment;

    segment = std::make_unique<boost::interprocess::managed_shared_memory>(
        bip::open_or_create, shm_name.c_str(), MEM_LENGTH);

    // Allocate allocator
    const ShmemAllocator alloc_inst(segment->get_segment_manager());

    // Construct unordered_map in shared memory
    MyMap *my_map = segment->construct<MyMap>("MyMap")(alloc_inst);

    // Add 10000 elements to unordered_map and measure time
    std::cout << "boost::unordered_map on shared_memory emplace ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < EMPLACE_NUM; ++i) {
            my_map->emplace(i, i);
        });

    // Look up 10000 elements in unordered_map and measure time
    std::cout << "boost::unordered_map on shared_memory at ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < AT_NUM; ++i) {
            my_map->at(i % EMPLACE_NUM);
        });

    // Destroy unordered_map in shared memory
    segment->destroy<MyMap>("MyMap");

    // Remove shared memory
    bip::shared_memory_object::remove("MySharedMemory");
}

int main()
{
    std::cout << "Emplace() ops : " << EMPLACE_NUM << " , At() ops : " << AT_NUM << std::endl;
    // test_std_unordered_map();
    // test_boost_unordered_map();
    test_shared_memory_boost_unordered_map();
    return 0;
}
