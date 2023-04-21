#include <iostream>
#include <thread>
#include <numa.h>
#include <boost/lockfree/queue.hpp>

const int QUEUE_LENGTH = 200;
// const int LOOP_NUM    = 1000;

void init_thread(boost::lockfree::queue<int, boost::lockfree::capacity<QUEUE_LENGTH>> *arr, int node_num)
{
    numa_run_on_node(node_num);
    int start = node_num * QUEUE_LENGTH;
    int end = start + QUEUE_LENGTH;
    for (int i = start; i < end; ++i)
    {
        std::cout << "Node " << node_num << " - Pushing... i = " << i << std::endl;
        while (!arr[node_num].push(i))
        {
            std::cout << "Node " << node_num << " - Error : Pushing... i = " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } // 等待插入成功
    }
}

void print_list(boost::lockfree::queue<int, boost::lockfree::capacity<QUEUE_LENGTH>> *arr, int node_num)
{
    numa_run_on_node(node_num);
    std::cout << "Thread " << node_num << ":" << std::endl;

    int value = 0;
    auto start_time1 = std::chrono::steady_clock::now(); // 开始计时访问当前节点

    for (int i = 0; i < QUEUE_LENGTH; i++)
    {
        while (arr[node_num].pop(value))
        {
            std::cout << "Node " << node_num << " - Error : Popping... i = " << value << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } // 遍历弹出当前节点的所有元素
    }

    auto end_time1 = std::chrono::steady_clock::now();
    auto elapsed_time1 = std::chrono::duration_cast<std::chrono::microseconds>(end_time1 - start_time1).count(); // 计算时间差

    std::cout << "Node " << node_num << " <-> Node 0 :";
    std::cout << "Elapsed time1: " << elapsed_time1 << " microseconds" << std::endl;

    auto start_time2 = std::chrono::steady_clock::now(); // 开始计时访问另一节点

    for (int i = 0; i < QUEUE_LENGTH; i++)
    {
        while (arr[1 - node_num].pop(value))
        {
            std::cout << "Node " << node_num << " - Error : Popping... i = " << value << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } // 遍历弹出另一节点的所有元素
    }

    auto end_time2 = std::chrono::steady_clock::now();                                                           // 结束计时访问另一节点
    auto elapsed_time2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time2 - start_time2).count(); // 计算时间差

    std::cout << "Node " << node_num << " <-> Node 1 :";
    std::cout << "Elapsed time2: " << elapsed_time2 << " microseconds" << std::endl;
}

int main()
{
    const int num_numa_nodes = 2;
    boost::lockfree::queue<int, boost::lockfree::capacity<QUEUE_LENGTH>> arr[num_numa_nodes];
    std::cout << "Begin initialization ..." << std::endl;

    std::thread t1(init_thread, arr, 0);
    std::thread t2(init_thread, arr, 1);
    t1.join();
    t2.join();
    std::cout << "Finish initialization" << std::endl;

    std::thread t3(print_list, arr, 0);
    t3.join();
    std::cout << "Node 0 popped..." << std::endl;

    std::cout << "Begin initialization again ..." << std::endl;
    std::thread t4(init_thread, arr, 0);
    std::thread t5(init_thread, arr, 1);
    t1.join();
    t2.join();
    std::cout << "Finish initialization" << std::endl;

    std::thread t6(print_list, arr, 1);
    t4.join();
    std::cout << "Node 1 popped..." << std::endl;

    return 0;
}
