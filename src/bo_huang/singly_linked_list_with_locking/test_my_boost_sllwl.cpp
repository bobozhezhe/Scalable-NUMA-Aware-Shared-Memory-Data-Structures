#include "../lib/singly_linked_list_with_locking.h"

#include <boost/mpi.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <vector>

using namespace boost::interprocess;
using namespace boost::mpi;

int main(int argc, char** argv) {

const int TIMES = 1000000;
const int NUM_PROCESSES = 4;
const int MEM_LENGTH = 65536;

// Initialize MPI
boost::mpi::environment env(argc, argv);
boost::mpi::communicator comm;

int rank = comm.rank();
// Define the shared memory name
std::string shm_name = "my_shared_memory";
// Create or open the shared memory segment
shared_memory_object::remove(shm_name.c_str());
boost::interprocess::managed_shared_memory segment(open_or_create, shm_name.c_str(), MEM_LENGTH);

// Define an allocator for the [linked list]
typedef boost::interprocess::allocator<singly_linked_list_with_locking *, managed_shared_memory::segment_manager> ShmemAllocator;

// Define the linked list
typedef SinglyLinkedListWithLocking<ShmemAllocator> MyLinkedList;

// Construct the linked list in the shared memory
MyLinkedList* list = segment.construct<MyLinkedList>("my_list")(segment.get_segment_manager());

double start_time = MPI_Wtime();
// Distribute [insert operations](poe://www.poe.com/_api/key_phrase?phrase=insert%20operations&prompt=Tell%20me%20more%20about%20insert%20operations.) across all processes
for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
    list->insert(i);
}
double end_time = MPI_Wtime();
double elapsed_time = end_time - start_time;
double max_elapsed_time;

boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

if (comm.rank() == 0) {
    std::cout << "Rank 0: Insert elapsed time: " << max_elapsed_time << " seconds" << std::endl;
}
else if (comm.rank() == 1) {
    std::cout << "Rank 1: Insert elapsed time: " << max_elapsed_time << " seconds" << std::endl;
}

boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());

start_time = MPI_Wtime();
// Distribute search operations across all processes
for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
    list->search(i);
}
end_time = MPI_Wtime();

elapsed_time = end_time - start_time;
boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

if (comm.rank() == 0) {
    std::cout << "Rank 0: Search elapsed time: " << max_elapsed_time << " seconds" << std::endl;
}
else if (comm.rank() == 1) {
    std::cout << "Rank 1: Search elapsed time: " << max_elapsed_time << " seconds" << std::endl;
}

// Destroy the linked list and the shared memory segment
segment.destroy<MyLinkedList>("my_list");
shared_memory_object::remove(shm_name.c_str());

return 0;
}
