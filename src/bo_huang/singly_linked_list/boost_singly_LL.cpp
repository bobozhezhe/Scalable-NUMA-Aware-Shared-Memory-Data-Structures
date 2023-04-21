/*
This program demonstrates the usage of Boost Intrusive SList data structure to create a linked list
and measure the performance of accessing the nodes in different NUMA nodes.
Boost Intrusive SList is a linked list implementation that provides a hook mechanism for linking objects
into the list, without requiring inheritance from a specific base class.
The program creates a linked list with two NUMA nodes, and initializes them with 10000 elements each.
Then, two threads access the nodes in each NUMA node alternatively for 100000 times, and measure the time
it takes to access the nodes.
The program uses the numa library to set the current thread's affinity to a specific NUMA node, and measure
the elapsed time using the std::chrono library.
*/
#include <iostream>
#include <thread>
#include <numa.h>
#include <boost/intrusive/slist.hpp>

const int LIST_LENGTH = 10000;
const int LOOP_NUM = 100000;

// Define the linked list node
class ListNode : public boost::intrusive::slist_base_hook<>
{
public:
    int val;
    ListNode(int x) : val(x) {}
};

// Define the linked list type
using List = boost::intrusive::slist<ListNode>;

// Initialize the linked list on a specific NUMA node
void init_thread(List *arr, int node_num)
{
    numa_run_on_node(node_num);
    int start = node_num * LIST_LENGTH;
    int end = start + LIST_LENGTH;
    for (int i = start; i <= end; ++i)
    {
        ListNode *node_ptr = new ListNode{i};
        arr[node_num].push_front(*node_ptr);
    }
}

// Access the linked list nodes on a specific NUMA node and measure the elapsed time
void print_list(List *arr, int node_num)
{
    numa_run_on_node(node_num);
    std::cout << "Thread " << node_num << ":" << std::endl;

    int value = 0;
    auto start_time1 = std::chrono::steady_clock::now(); // Start measuring the elapsed time of accessing the nodes in the same node

    for (int i = 0; i < LOOP_NUM; ++i)
    {
        for (auto &node : arr[node_num])
        {
            value = node.val;
        }
    }

    auto end_time1 = std::chrono::steady_clock::now();                                                           // End measuring the elapsed time of accessing the nodes in the same node
    auto elapsed_time1 = std::chrono::duration_cast<std::chrono::microseconds>(end_time1 - start_time1).count(); // Calculate the elapsed time

    std::cout << "Node " << node_num << " <-> Node " << node_num;
    std::cout << " - Elapsed time: " << elapsed_time1 << " microseconds" << std::endl;

    auto start_time2 = std::chrono::steady_clock::now(); // Start measuring the elapsed time of accessing the nodes in the other node

    int other_node = 1 - node_num;
    for (int i = 0; i < LOOP_NUM; ++i)
    {
        for (auto &node : arr[other_node])
        {
            value = node.val;
        }
    }

    auto end_time2 = std::chrono::steady_clock::now();                                                           // End measuring the elapsed time of accessing the nodes in the other node
    auto elapsed_time2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time2 - start_time2).count(); // Calculate the elapsed time

    std::cout << "Node " << node_num << " <-> Node " << other_node;
    std::cout << " - Elapsed time: " << elapsed_time2 << " microseconds" << std::endl;
}

// The main function initializes the linked list and measures the access time in different NUMA nodes
int main()
{
    const int num_numa_nodes = 2;
    List arr[num_numa_nodes];
    std::cout << "Begin initialization ..." << std::endl;

    // Initialize the linked list on two different NUMA nodes using two threads
    std::thread t1(init_thread, &arr[0], 0);
    std::thread t2(init_thread, &arr[0], 1);

    t1.join();
    t2.join();

    std::cout << "Finish initialization." << std::endl;

    // Access the linked list nodes on two different NUMA nodes using two threads
    std::thread t3(print_list, &arr[0], 0);
    std::thread t4(print_list, &arr[0], 1);

    t3.join();
    t4.join();

    return 0;
}