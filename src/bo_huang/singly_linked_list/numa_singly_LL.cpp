// #include <boost/mpi.hpp>
// #include <boost/unordered_map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
// #include <boost/interprocess/allocators/allocator.hpp>
// #include <boost/serialization/vector.hpp>
// #include <boost/serialization/access.hpp>
// #include <boost/serialization/base_object.hpp>
// #include <boost/archive/text_iarchive.hpp>
// #include <boost/archive/text_oarchive.hpp>
// #include <iostream>
// #include <vector>
// #include <thread>
// #include <memory>
// #include <numa.h>
#include <boost/interprocess/containers/slist.hpp>
// #include <sys/mman.h>


#include "singly_linked_list.h"

// using namespace boost::interprocess;
// using namespace boost::mpi;
namespace bip = boost::interprocess;

// void thread_func(int node)
// {
//     // 将线程绑定到指定的NUMA节点上
//     numa_run_on_node(node);

//     // 输出线程的ID和所在的NUMA节点
//     std::cout << "Thread " << std::this_thread::get_id() << " bound to node " << node << std::endl;

//     // 在这里可以执行与NUMA相关的操作，例如分配内存等
//     // ...

//     // 等待一段时间，然后退出线程
//     std::this_thread::sleep_for(std::chrono::seconds(1));
// }


int main(int argc, char **argv)
{
    const int NUM_LIST = 10;
    const int LOOP_NUM = 10000;
    size_t MEM_SIZE = 1 * 1024 * 1024; // 256MB

    MPI_Init(&argc, &argv);

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";

    // Define the shared memory name
    // Get the node ID for NUMA node 1
    // int node_id = 0;

    // Allocate shared memory on NUMA node 1
    // void *ptr1 = numa_alloc_onnode(MEM_SIZE, node_id);

    // Attach the shared memory to a Boost managed_shared_memory
    // bip::managed_shared_memory segment(bip::open_or_create, "MySharedMemory", MEM_SIZE, ptr1);
    bip::managed_shared_memory segment(bip::open_or_create, "MySharedMemory", MEM_SIZE);

    comm.barrier();

    std::cout << "Memory created..." << std::endl;

    // Create an allocator for the list elements
    typedef bip::allocator<int, bip::managed_shared_memory::segment_manager> allocator_t;
    allocator_t allocator(segment.get_segment_manager());

    // Create a singly linked list in the shared memory
    typedef bip::slist<int, allocator_t> list_t;
    list_t *list = segment.find_or_construct<list_t>("MyList")(allocator);

    std::cout << "Singly Linked List created..." << std::endl;

    // Add some elements to the list
    for (int i = 0; i < NUM_LIST; ++i)
    {
        list->push_front(i);
    }

    std::cout << "Singly Linked List inserted..." << std::endl;

    // list->push_front(3);
    // list->push_front(2);
    // list->push_front(1);

    // Print the elements in the list
    std::cout << "Rank " << rank << ": ";
    TIME_MEASURE_MPI(
        for (int i = 0; i < LOOP_NUM; ++i) {
            for (auto it = list->begin(); it != list->end(); ++it) {
                std::cout << *it << std::endl;
            }
        });

    // Deallocate the shared memory segment
    segment.destroy<list_t>("MyList");
    // bip::managed_shared_memory::remove("MySharedMemory");
    bip::shared_memory_object::remove("MySharedMemory");
    // numa_free(ptr1, MEM_SIZE);
    MPI_Finalize();

    return 0;
}
