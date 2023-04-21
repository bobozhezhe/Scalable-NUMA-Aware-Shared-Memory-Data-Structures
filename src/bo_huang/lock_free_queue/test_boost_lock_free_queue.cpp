#include <boost/mpi.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <numa.h>

#include "../singly_linked_list/singly_linked_list.h"

// using namespace boost::interprocess;
// using namespace boost::mpi;
namespace bip = boost::interprocess;

int main(int argc, char **argv)
{

    // const int TIMES = 200;
    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 1 * (1<<20);
    const int CAP_QUEUE = 200;

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment
    bip::shared_memory_object::remove(shm_name.c_str());

    // boost::interprocess::managed_shared_memory segment(open_or_create, shm_name.c_str(), MEM_LENGTH);
    std::unique_ptr<boost::interprocess::managed_shared_memory> segment;

    int node_id = 0;
    void *ptr1 = numa_alloc_onnode(MEM_LENGTH, node_id);

    if (rank == 0)
    {
        segment = std::make_unique<boost::interprocess::managed_shared_memory>(
            bip::open_or_create, shm_name.c_str(), MEM_LENGTH, ptr1);
    }
    comm.barrier();
    if (rank != 0)
    {
        segment = std::make_unique<boost::interprocess::managed_shared_memory>(
            bip::open_or_create, shm_name.c_str(), MEM_LENGTH);
    }

    // Define an allocator for the lock-free queue
    typedef boost::interprocess::allocator<int, bip::managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the lock-free queue
    typedef boost::lockfree::queue<int, boost::lockfree::capacity<CAP_QUEUE>, boost::lockfree::allocator<ShmemAllocator>> MyLockFreeQueue;

    std::cout << "Finish... build queue." << std::endl;
    // Construct the lock-free queue in the shared memory
    MyLockFreeQueue *queue;
    if (rank == 0) {
        queue = segment->construct<MyLockFreeQueue>("my_queue")(segment->get_segment_manager());
    }
    comm.barrier();
    if (rank != 0) {
        queue = segment->find_or_construct<MyLockFreeQueue>("my_queue")(segment->get_segment_manager());
    }

    std::cout << "Finish... queue construct." << std::endl;
    // std::cout << "Queue size = ."<< queue->capacity() << std::endl;

    double start_time = MPI_Wtime();
    // Distribute push operations across all processes

    if (rank == 0)
    {
        for (int i = 0; i < CAP_QUEUE; i++)
        {
            std::cout << "Rank "<< rank <<" - Pushing... i = " << i << std::endl;

            // queue->push(i);
            while (!queue->push(i))
            {
                std::cout << "Pushing... i = " << i << std::endl;
            }
        }
        std::cout << "Finish ... Pushing" << std::endl;
    }
    comm.barrier();

    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;

    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    std::cout << "Rank "<< rank << ": Push elapsed time: " << max_elapsed_time << " seconds" << std::endl;

    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());

    start_time = MPI_Wtime();
    // Distribute pop operations across all processes
    int value;
    for (int i = rank; i < CAP_QUEUE; i += NUM_PROCESSES)
    {
        while (!queue->pop(value))
        {
            std::cout << "Error ... Popping... i = " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Rank "<< rank <<" - Popping... i = " << i << std::endl;
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    std::cout << "Finish ... Popping" << std::endl;

    std::cout << "Rank "<< rank << ": Pop elapsed time: " << max_elapsed_time << " seconds" << std::endl;

    if (comm.rank() == 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // Destroy the lock-free queue and the shared memory segment
        segment->destroy<MyLockFreeQueue>("my_queue");
        bip::shared_memory_object::remove(shm_name.c_str());
        std::cout << "Rank 0: destroyed memory." << std::endl;
    }

    return 0;
}
