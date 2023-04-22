
#include <iostream>
#include <chrono>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
// #include <boost/intrusive/slist.hpp>
#include <boost/container/slist.hpp>

#include "../lib/numa_experience.h"

const int WRITE_NUM = 100;
const int LOOP_NUM = 10000;
const int READ_NUM = WRITE_NUM * LOOP_NUM;
const int MEM_LENGTH = 256 * (1 << 10);

// const int LIST_LENGTH = 10000;

#include <forward_list>

void test_std_local()
{
    // 初始化
    std::forward_list<int> l;
    // std::unordered_map<int, int> my_list;
    std::cout << "std::forward_list push_front() ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < WRITE_NUM; i++) {
            // std::cout << "push_front... i = " << i << std::endl;
            l.push_front(i);
        });

    int value = 0;
    std::cout << "std::forward_list traverse ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < LOOP_NUM; i++) {
            for (auto it = l.begin(); it != l.end(); ++it)
            {
                value = *it;
            }
        });
}

// Define the linked list node
class ListNode : public boost::intrusive::slist_base_hook<>
{
public:
    int val;
    ListNode(int x) : val(x) {}
};

// Define the linked list type
using List = boost::intrusive::slist<ListNode>;

void test_boost_local()
{

    // 初始化
    List *arr = new List();

    std::cout << "boost::unordered_map emplace ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < WRITE_NUM; i++) {
            ListNode *node_ptr = new ListNode{i};
            arr->push_front(*node_ptr);
        });

    int value = 0;
    std::cout << "boost::unordered_map at ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < LOOP_NUM; ++i) {
            for (auto node : *arr)
            {
                value = node.val;
            }
        }

    );
}

// // using namespace boost::interprocess;
namespace bip = boost::interprocess;

void test_shared_memory_boost()
{
    // typedef bip::allocator<std::pair<const int, int>, bip::managed_shared_memory::segment_manager> ShmemAllocator;
    typedef bip::allocator<int, bip::managed_shared_memory::segment_manager> ShmemAllocator;
    // typedef boost::unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MySlist;
    typedef boost::container::slist<int, ShmemAllocator> MySlist;

    // const int NUM_PROCESSES = 4;

    // Initialize MPI
    boost::mpi::environment env();
    boost::mpi::communicator comm;

    std::string shm_name = "my_shared_memory";
    // Remove shared memory if it already exists
    bip::shared_memory_object::remove(shm_name.c_str());

    // Create shared memory
    // managed_shared_memory segment(create_only, "MySharedMemory", 65536);
    // std::unique_ptr<boost::interprocess::managed_shared_memory> segment;

    // segment = std::make_unique<boost::interprocess::managed_shared_memory>(
    // bip::open_or_create, shm_name.c_str(), MEM_LENGTH);

    // bip::managed_shared_memory segment(bip::create_only, "MySharedMemory", 65536);

    // Allocate allocator
    // const ShmemAllocator alloc_inst(segment.get_segment_manager());

    // Construct unordered_map in shared memory
    // MySlist *slist = segment.construct<MySlist>("MySlist")(alloc_inst);

    // 创建共享内存
    bip::managed_shared_memory segment(bip::create_only, shm_name.c_str(), MEM_LENGTH);

    // 定义分配器类型
    typedef bip::allocator<int, bip::managed_shared_memory::segment_manager> ShmemAllocator;

    // 定义 slist 类型
    typedef boost::container::slist<int, ShmemAllocator> MySlist;

    // 在共享内存中创建 slist
    MySlist *slist = segment.construct<MySlist>("MySlist")(segment.get_segment_manager());

    // Add 10000 elements to unordered_map and measure time
    std::cout << "boost::container::slist on shared_memory push_front ";

    MEASURE_TIME_HIGH_RESOLUTION(

        for (int i = 0; i < WRITE_NUM; ++i) {
            slist->push_front(i);
        }

    );

    // Look up 10000 elements in unordered_map and measure time
    int value = 0;
    std::cout << "boost::container::slist on shared_memory traverse ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < LOOP_NUM; ++i) {
            //     my_list->at(i % WRITE_NUM);
            for (auto it = slist->begin(); it != slist->end(); ++it)
            {
                value = *it;
            }
        });

    // Destroy unordered_map in shared memory
    segment.destroy<MySlist>("MySlist");

    // Remove shared memory
    bip::shared_memory_object::remove("MySharedMemory");
}

int main()
{
    std::cout << "Singly_linked_list write ops : " << WRITE_NUM << " , read ops : " << READ_NUM << std::endl;
    test_std_local();
    test_boost_local();
    test_shared_memory_boost();
    return 0;
}
