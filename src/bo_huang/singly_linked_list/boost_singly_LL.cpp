
#include <iostream>
#include <boost/intrusive/slist.hpp>
#include "singly_linked_list.h"

// #include <boost/mpi.hpp>
// #define TIME_MEASURE_MPI(loop_code)                                                 \
//     do                                                                              \
//     {                                                                               \
//         double start_time = MPI_Wtime();                                            \
//         loop_code;                                                                  \
//         double end_time = MPI_Wtime();                                              \
//         double elapsed_time = end_time - start_time;                                \
//         std::cout << "- elapsed time: " << elapsed_time << " seconds" << std::endl; \
//     } while (0)

// // 定义链表节点
// class ListNode : public boost::intrusive::slist_base_hook<>
// {
// public:
//     int val;
//     ListNode(int x) : val(x) {}
// };

// 定义链表类型
// using List = boost::intrusive::slist<ListNode>;

int main()
{
    const int NUM_LIST = 10000;
    const int LOOP_NUM = 10000;

    List list;

    // 在链表头部插入节点
    for (int i = 0; i < NUM_LIST; ++i)
    {
        ListNode *node_ptr = new ListNode(i);
        list.push_front(*node_ptr);
    }

    // 遍历链表
    std::cout << "Access " << NUM_LIST * LOOP_NUM << " times: ";

    TIME_MEASURE_MPI(
        for (int i = 0; i < LOOP_NUM; ++i) {
            for (auto it = list.begin(); it != list.end(); ++it)
            {
                ListNode *node_ptr = &(*it);
                // std::cout << node_ptr->val << " ";
                int dummy = node_ptr->val;
            }
        });

    return 0;
}
