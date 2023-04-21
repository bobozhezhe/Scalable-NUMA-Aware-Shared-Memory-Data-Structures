
#include <iostream>
#include <thread>
#include <numa.h>
#include <boost/intrusive/slist.hpp>

const int LIST_LENGTH = 10000;
const int LOOP_NUM    = 100000;

// 定义链表节点
class ListNode : public boost::intrusive::slist_base_hook<>
{
public:
    int val;
    ListNode(int x) : val(x) {}
};

// 定义链表类型
using List = boost::intrusive::slist<ListNode>;

void init_thread(List* arr, int node_num) {
    numa_run_on_node(node_num);
    int start = node_num * LIST_LENGTH;
    int end = start + LIST_LENGTH;
    for (int i = start; i <= end; ++i) {
        ListNode* node_ptr = new ListNode{i};
        arr[node_num].push_front(*node_ptr);
    }
}


void print_list(List* arr, int node_num) {
    numa_run_on_node(node_num);
    std::cout << "Thread " << node_num << ":" << std::endl;

    int value = 0;
    auto start_time1 = std::chrono::steady_clock::now(); // 开始计时访问当前节点

    for (int i = 0; i < LOOP_NUM; ++i) {
        for (auto& node : arr[node_num]) {
            value = node.val;
        }
    }

    auto end_time1 = std::chrono::steady_clock::now(); // 结束计时访问当前节点
    auto elapsed_time1 = std::chrono::duration_cast<std::chrono::microseconds>(end_time1 - start_time1).count(); // 计算时间差

    std::cout << "Node " << node_num << " <-> Node 0 :";
    std::cout << "Elapsed time1: " << elapsed_time1 << " microseconds" << std::endl;

    auto start_time2 = std::chrono::steady_clock::now(); // 开始计时访问另一节点

    for (int i = 0; i < 10000; ++i) {
        for (auto& node : arr[1 - node_num]) {
            value = node.val;
        }
    }

    auto end_time2 = std::chrono::steady_clock::now(); // 结束计时访问另一节点
    auto elapsed_time2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time2 - start_time2).count(); // 计算时间差

    std::cout << "Node " << node_num << " <-> Node 1 :";
    std::cout << "Elapsed time2: " << elapsed_time2 << " microseconds" << std::endl;
}

int main() {
    const int num_numa_nodes = 2;
    List arr[num_numa_nodes];
    std::cout << "Before initialization:" << std::endl;

    std::thread t1(init_thread, &arr[0], 0);
    std::thread t2(init_thread, &arr[0], 1);

    t1.join();
    t2.join();

    std::cout << "Finish initialization:" << std::endl;

    std::thread t3(print_list, &arr[0], 0);
    std::thread t4(print_list, &arr[0], 1);

    t3.join();
    t4.join();

    return 0;
}