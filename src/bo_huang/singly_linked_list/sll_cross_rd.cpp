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
#include <numa.h>
#include <sstream>
#include <boost/interprocess/sync/named_mutex.hpp>

namespace bip = boost::interprocess;

const int MEM_LENGTH = 256 * (1 << 20);

#define MEASURE_TIME_MPI(loop_code)                                                                 \
    do                                                                                              \
    {                                                                                               \
        double max_elapsed_time;                                                                    \
        double start_time = MPI_Wtime();                                                            \
        loop_code;                                                                                  \
        double end_time = MPI_Wtime();                                                              \
        double elapsed_time = end_time - start_time;                                                \
        boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0); \
        std::cout << "- elapsed time: " << max_elapsed_time << " seconds" << std::endl;             \
    } while (0)

template <typename T>
int test_shm_cross_slist(int loop_num, std::vector<double> &times, boost::mpi::communicator *pComm)
{

    const int TIMES = loop_num;
    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 256 * (1 << 20); // 256MB
    const int MAP_LENGTH = 1024;

    int rank = pComm->rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment

    if (rank == 0)
    {
        bip::shared_memory_object::remove(shm_name.c_str());
    }
    pComm->barrier();

    std::unique_ptr<boost::interprocess::managed_shared_memory> segment;
    // std::unique_ptr<boost::interprocess::named_mutex> mutex;

    // boost::interprocess::named_mutex m_mutex(boost::interprocess::open_or_create, "my_named_mutex"), *mutex = &m_mutex;

    if (rank == 0)
    {
        segment = std::make_unique<boost::interprocess::managed_shared_memory>(bip::open_or_create, shm_name.c_str(), MEM_LENGTH);
        // mutex = std::make_unique<boost::interprocess::named_mutex>(bip::open_or_create, "named_mutex_name");
    }
    pComm->barrier();
    if (rank != 0)
    {
        segment = std::make_unique<boost::interprocess::managed_shared_memory>(bip::open_or_create, shm_name.c_str(), MEM_LENGTH);
        // mutex = std::make_unique<boost::interprocess::named_mutex>(bip::open_or_create, "named_mutex_name");
    }

    // Define an allocator for the unordered_map
    // typedef boost::interprocess::allocator<std::pair<const int, int>, bip::managed_shared_memory::segment_manager> ShmemAllocator;
    typedef bip::allocator<int, bip::managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the unordered_map
    // typedef boost::unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyHashMap;
    typedef boost::container::slist<int, ShmemAllocator> MySlist;

    // Construct the unordered_map in the shared memory
    // MyHashMap *map;
    MySlist *slist;
    // bip::named_mutex* mutex;

    if (rank == 0)
    {
        slist = segment->construct<MySlist>("my_list")(segment->get_segment_manager());
        // mutex = segment->find_or_construct<bip::named_mutex>("mutex")(bip::open_or_create, "named_mutex_name");
    }
    pComm->barrier();
    if (rank != 0)
    {
        slist = segment->find_or_construct<MySlist>("my_list")(segment->get_segment_manager());
        // mutex = segment->find_or_construct<bip::named_mutex>("mutex")(bip::open_or_create, "named_mutex_name");
    }

    // get the cpu id and node id
    int cpu = sched_getcpu();
    int node = numa_node_of_cpu(cpu);

    double start_time = MPI_Wtime();
    // Distribute emplace operations across all processes
    if (rank == 0)
    {
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "building slist ... " << std::endl;
        std::cout << "boost::container::slist on shared_memory push_front ";
        // MEASURE_TIME_MPI(
        start_time = MPI_Wtime();
        for (int i = 0; i < TIMES; ++i)
        {
            slist->push_front(i);
            // map->emplace(i, i);
        }
        // );
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    double max_elapsed_time_ms = elapsed_time * 1000 * 1000;
    times.push_back(max_elapsed_time_ms);
    std::cout << "[Rank " << rank << " on node " << node << ":] ";
    std::cout << "Emplace elapsed time: " << max_elapsed_time_ms << " microseconds" << std::endl;

    pComm->barrier();
    double max_elapsed_time;
    boost::mpi::reduce(*pComm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);
    boost::mpi::all_reduce(*pComm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());
    std::cout << "Emplace barrier elapsed time: " << max_elapsed_time * 1000 * 1000 << " microseconds" << std::endl;

    // Acquire the mutex
    // mutex->lock();
    // std::cout << "[Rank " << rank << " on node " << node << ":] ";
    // std::cout << "Get mutex_lock ... " << std::endl;

    if (rank == 0)
    {
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "Begin read ... ";
        start_time = MPI_Wtime();
        // Distribute get operations across all processes
        for (auto it = slist->begin(); it != slist->end(); ++it)
        {
            int value = *it;
        }
        end_time = MPI_Wtime();
        std::cout << "Finish read ... " << std::endl;
    }
    pComm->barrier();
    if (rank == 1)
    {
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "Begin read ... ";
        start_time = MPI_Wtime();
        // Distribute get operations across all processes
        for (auto it = slist->begin(); it != slist->end(); ++it)
        {
            int value = *it;
        }
        end_time = MPI_Wtime();
        std::cout << "Finish read ... " << std::endl;
    }
    pComm->barrier();
    if (rank == 2)
    {
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "Begin read ... ";
        start_time = MPI_Wtime();
        // Distribute get operations across all processes
        for (auto it = slist->begin(); it != slist->end(); ++it)
        {
            int value = *it;
        }
        end_time = MPI_Wtime();
        std::cout << "Finish read ... " << std::endl;
    }
    pComm->barrier();
    if (rank == 3)
    {
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "Begin read ... ";
        start_time = MPI_Wtime();
        // Distribute get operations across all processes
        for (auto it = slist->begin(); it != slist->end(); ++it)
        {
            int value = *it;
        }
        end_time = MPI_Wtime();
        std::cout << "Finish read ... " << std::endl;
    }
    pComm->barrier();

    // Release the mutex
    // mutex->unlock();
    // std::cout << "[Rank " << rank << " on node " << node << ":] ";
    // std::cout << "Unclock mutex_lock ... " << std::endl;

    elapsed_time = end_time - start_time;

    max_elapsed_time_ms = elapsed_time * 1000 * 1000;
    times.push_back(max_elapsed_time_ms);

    // Read performance test
    std::cout << "[Rank " << rank << " on node " << node << ":] ";
    std::cout << "boost::container::slist on shared_memory read ";
    std::cout << "Getting key elapsed time: " << max_elapsed_time_ms << " microseconds" << std::endl;

    pComm->barrier();
    boost::mpi::reduce(*pComm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);
    boost::mpi::all_reduce(*pComm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());
    std::cout << "Read barrier elapsed time: " << max_elapsed_time * 1000 * 1000 << " microseconds" << std::endl;

    std::cout << "[Rank " << rank << " on node " << node << ":] ";
    std::cout << "begin to erase.... " << std::endl;
    // Deletion performance test
    start_time = MPI_Wtime();
    if (rank == 0)
    {
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "boost::container::slist on shared_memory erase ";
        // MEASURE_TIME_MPI(
        for (auto it = slist->begin(); it != slist->end();)
        {
            it = slist->erase(it);
        }
        // );
    };
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;

    max_elapsed_time_ms = elapsed_time * 1000 * 1000;
    times.push_back(max_elapsed_time_ms);
    std::cout << "Getting key elapsed time: " << max_elapsed_time_ms << " microseconds" << std::endl;

    std::cout << "[Rank " << rank << " on node " << node << ":] ";
    std::cout << "Finish erase.... " << std::endl;

    pComm->barrier();
    boost::mpi::reduce(*pComm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);
    std::cout << "Erase barrier elapsed time: " << max_elapsed_time * 1000 * 1000 << " microseconds" << std::endl;

    if (rank == 0)
    {
        // std::sleep(3);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // Destroy the unordered_map and the shared memory segment
        segment->destroy<MySlist>("my_list");
        bip::shared_memory_object::remove(shm_name.c_str());
        std::cout << "[Rank " << rank << " on node " << node << ":] ";
        std::cout << "Destroyed memory." << std::endl;
    }

    return 0;
}

// boost::mpi::communicator *pComm;

int main(int argc, char **argv)
{
    std::vector<double> times;
    // int loop_num = 1000;

    const int kNumTests = 4;
    constexpr int kNumIters[kNumTests] = {1000, 10000, 100000, 1000000};

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    // Initialize MPI
    boost::mpi::communicator comm;
    // open csv file
    int rank = comm.rank();
    // get the cpu id and node id
    int cpu = sched_getcpu();
    int node = numa_node_of_cpu(cpu);
    std::stringstream filename;
    filename << "../../../data/cross_rd_results_rank_" << rank << "_on_node_" << node << ".csv";
    std::ofstream file(filename.str());

    // test
    int row = 0;
    // table header
    file << "container,datatype,operation,loop_num,time(us)" << std::endl;
    for (int i = 0; i < kNumTests; i++)
    {
        int loop_num = kNumIters[i];
        std::cout << "boost::container::slist on shared_memory loop = " << loop_num << std::endl;

        test_shm_cross_slist<int>(loop_num, times, &comm);
        test_shm_cross_slist<double>(loop_num, times, &comm);
        test_shm_cross_slist<std::vector<int>>(loop_num, times, &comm);

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
