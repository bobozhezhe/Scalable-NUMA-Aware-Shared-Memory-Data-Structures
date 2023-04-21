#include <boost/mpi.hpp>
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/slist.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/timer/timer.hpp>

#include "singly_linked_list.h"

using namespace boost::interprocess;
// using namespace boost::mpi;

// Define the data structure to store in the slist
struct my_data
{
    int id;
    double value;
};

// Define the allocator type for the slist
typedef allocator<my_data, managed_shared_memory::segment_manager> my_data_allocator;

int main(int argc, char **argv)
{

    const int NUM_LIST = 20;

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    int rank = comm.rank();
    std::string shm_name = "my_shared_memory";
    // Create a shared memory object with a size of 1MB
    shared_memory_object::remove(shm_name.c_str());
    comm.barrier();

    managed_shared_memory segment(create_only, shm_name.c_str(), 1 << 20);

    // Construct the slist in the shared memory with the custom allocator
    slist<my_data, my_data_allocator> *my_list = segment.construct<slist<my_data, my_data_allocator>>("my_list")(segment.get_segment_manager());

    // Add some data to the slist
    for (int i = 0; i < NUM_LIST; ++i)
    {
        my_list->push_front({i, (double)i});
    }

    // Read the data from the slist and measure the time taken
    // double start_time = MPI_Wtime();
    TIME_MEASURE_MPI(
        for (auto it = my_list->begin(); it != my_list->end(); ++it) {
            std::cout << "id: " << it->id << ", value: " << it->value << std::endl;
        });
    // double end_time = MPI_Wtime();
    // double elapsed_time = end_time - start_time;
    // double max_elapsed_time;
    // boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (comm.rank() == 0)
    {
        // std::cout << "Rank 0: Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (comm.rank() == 1)
    {
        // std::cout << "Rank 1: Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    // Destroy the slist and the shared memory segment
    segment.destroy<slist<my_data, my_data_allocator>>("my_list");
    shared_memory_object::remove("my_shared_memory");

    return 0;
}
