#include <boost/mpi.hpp>
#include <boost/unordered_set.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include <thread>

using namespace boost::interprocess;
using namespace boost::mpi;

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int TIMES = 1000000;
    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 65536;
    const int SET_LENGTH = 1024;

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    // int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment
    shared_memory_object::remove(shm_name.c_str());
    boost::interprocess::managed_shared_memory segment(open_or_create, shm_name.c_str(), MEM_LENGTH);

    // Define an allocator for the unordered_set
    typedef boost::interprocess::allocator<int, managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the unordered_set
    typedef boost::unordered_set<int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyHashSet;

    // Construct the unordered_set in the shared memory
    MyHashSet* set = segment.construct<MyHashSet>("my_set")(segment.get_segment_manager());


    double start_time = MPI_Wtime();
    // Distribute insert operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        set->insert(i % SET_LENGTH);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;

    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (rank == 0) {
        std::cout << "Rank 0: Insert elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (rank == 1) {
        std::cout << "Rank 1: Insert elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());


    start_time = MPI_Wtime();
    // Distribute find operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        MyHashSet::const_iterator iter = set->find(i % SET_LENGTH);
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (rank == 0) {
        std::cout << "Rank 0: Finding element elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (rank == 1) {
        std::cout << "Rank 1: Finding element elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }


    if (rank == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Destroy the unordered_set and the shared memory segment
        segment.destroy<MyHashSet>("my_set");
        shared_memory_object::remove(shm_name.c_str());
        MPI_Finalize();
    }

    return 0;
}
