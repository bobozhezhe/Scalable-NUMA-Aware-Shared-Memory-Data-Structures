#include <boost/mpi.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>

using namespace boost::interprocess;
using namespace boost::mpi;

int main(int argc, char** argv) {

    const int TIMES = 200;
    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 65536;
    const int CAP_QUEUE  = 100;

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment
    shared_memory_object::remove(shm_name.c_str());
    boost::interprocess::managed_shared_memory segment(open_or_create, shm_name.c_str(), MEM_LENGTH);

    // Define an allocator for the lock-free queue
    typedef boost::interprocess::allocator<int, managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the lock-free queue
    typedef boost::lockfree::queue<int, boost::lockfree::capacity<CAP_QUEUE>, boost::lockfree::allocator<ShmemAllocator>> MyLockFreeQueue;

    std::cout << "Finish... build queue." << std::endl;
    // Construct the lock-free queue in the shared memory
    MyLockFreeQueue* queue = segment.find_or_construct<MyLockFreeQueue>("my_queue")(segment.get_segment_manager());

    std::cout << "Finish... segment construct." << std::endl;

    double start_time = MPI_Wtime();
    // Distribute push operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        std::cout << "Pushing... i = " << i << std::endl;

        while (!queue->push(i)) {}
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;

    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (comm.rank() == 0) {
        std::cout << "Rank 0: Push elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (comm.rank() == 1) {
        std::cout << "Rank 1: Push elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());

    start_time = MPI_Wtime();
    // Distribute pop operations across all processes
    int value;
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        std::cout << "Popping... i = " << i << std::endl;
        while (!queue->pop(value)) {}
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (comm.rank() == 0) {
        std::cout << "Rank 0: Pop elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (comm.rank() == 1) {
        std::cout << "Rank 1: Pop elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    // Destroy the lock-free queue and the shared memory segment
    segment.destroy<MyLockFreeQueue>("my_queue");
    shared_memory_object::remove(shm_name.c_str());

    return 0;
}
