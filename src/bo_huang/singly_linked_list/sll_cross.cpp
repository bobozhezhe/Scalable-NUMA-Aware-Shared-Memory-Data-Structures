#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <forward_list>
#include <fstream>
#include <typeinfo>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/container/slist.hpp>
#include <boost/mpi.hpp>
#include <thread>
#include <boost/unordered_map.hpp>

namespace bip = boost::interprocess;

const int MEM_LENGTH = 256 * (1 << 20);

#define MEASURE_TIME_HIGH_RESOLUTION(loop_code)                                                                     \
    do                                                                                                              \
    {                                                                                                               \
        auto start_emplace = std::chrono::high_resolution_clock::now();                                             \
        loop_code;                                                                                                  \
        auto end_emplace = std::chrono::high_resolution_clock::now();                                               \
        auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace); \
        std::cout << "- elapsed time: " << duration_emplace.count() << " microseconds" << std::endl;                \
        times.push_back(duration_emplace.count());                                                                  \
    } while (0)

template <typename T>
int test_shm_cross_slist(int loop_num, std::vector<double> &times, int argc, char **argv)
{

    const int TIMES = loop_num;
    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 256 * (1 << 20); // 256MB
    const int MAP_LENGTH = 1024;

    // Initialize MPI
    boost::mpi::communicator comm;

    int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment

    if (rank == 0)
    {
        bip::shared_memory_object::remove(shm_name.c_str());
    }
    comm.barrier();

    std::unique_ptr<boost::interprocess::managed_shared_memory> segment;

    if (rank == 0)
    {
        segment = std::make_unique<boost::interprocess::managed_shared_memory>(
            bip::open_or_create, shm_name.c_str(), MEM_LENGTH);
    }
    comm.barrier();
    if (rank != 0)
    {
        segment = std::make_unique<boost::interprocess::managed_shared_memory>(
            bip::open_or_create, shm_name.c_str(), MEM_LENGTH);
    }

    // Define an allocator for the unordered_map
    typedef boost::interprocess::allocator<std::pair<const int, int>, bip::managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the unordered_map
    typedef boost::unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyHashMap;

    // Construct the unordered_map in the shared memory
    MyHashMap *map;
    if (rank == 0)
    {
        map = segment->construct<MyHashMap>("my_map")(
            segment->get_segment_manager());
    }
    comm.barrier();
    if (rank != 0)
    {
        map = segment->find_or_construct<MyHashMap>("my_map")(
            segment->get_segment_manager());
    }

    double start_time = MPI_Wtime();
    // Distribute emplace operations across all processes
    if (rank == 0)
    {
        for (int i = 0; i < TIMES; ++i)
        {
            map->emplace(i, i);
        }
    }
    comm.barrier();
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;

    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    // Synchronize the shared memory segment
    // segment->shmem_barrier::operator()(comm);

    std::cout << "Rank " << rank << ": Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;

    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());

    start_time = MPI_Wtime();
    // Distribute get operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES)
    {
        int value = (*map)[i % MAP_LENGTH];
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    std::cout << "Rank " << rank << ": Getting key elapsed time: " << max_elapsed_time << " seconds" << std::endl;

    if (comm.rank() == 0)
    {
        // std::sleep(3);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // Destroy the unordered_map and the shared memory segment
        segment->destroy<MyHashMap>("my_map");
        bip::shared_memory_object::remove(shm_name.c_str());
        std::cout << "Rank 0: destroyed memory.";
    }

    return 0;

}

int main(int argc, char **argv)
{
    std::vector<double> times;
    // int loop_num = 1000;

    const int kNumTests = 4;
    constexpr int kNumIters[kNumTests] = {1000, 10000, 1000, 10};
    std::ofstream file("shm_results.csv");

    // test
    int row = 0;
    // table header
    file << "container,datatype,operation,loop_num,time(us)" << std::endl;
    boost::mpi::environment env(argc, argv);
    for (int i = 0; i < kNumTests; i++)
    {
        int loop_num = kNumIters[i];
        std::cout << "boost::container::slist on shared_memory loop = " << loop_num << std::endl;

        test_shm_cross_slist<int>(loop_num, times, argc, argv);
        test_shm_cross_slist<double>(loop_num, times, argc, argv);
        test_shm_cross_slist<std::vector<int>>(loop_num, times, argc, argv);

        std::string container_names[1] = {"boost::intrusive::slist"};
        std::string datatype_names[3] = {typeid(int).name(), typeid(double).name(), typeid(std::vector<int>).name()};
        std::string operation_names[3] = {"insert", "read", "erase"};

        for (int container_num = 0; container_num < 1; container_num++)
        {
            for (int datatype_num = 0; datatype_num < 3; datatype_num++)
            {
                for (int op_num = 0; op_num < 3; op_num++)
                {
                    file << container_names[container_num] << "," << datatype_names[datatype_num] << "," << operation_names[op_num] << ",";
                    file << std::to_string(loop_num) << "," << times[row++] << std::endl;
                }
            }
        }
    }
    MPI_Finalize();

    file.close();

    return 0;
}
