
// #include <iostream>
#include <boost/intrusive/slist.hpp>
#include <boost/mpi.hpp>
#define TIME_MEASURE_MPI(loop_code)                                                 \
    do                                                                              \
    {                                                                               \
        double start_time = MPI_Wtime();                                            \
        loop_code;                                                                  \
        double end_time = MPI_Wtime();                                              \
        double elapsed_time = end_time - start_time;                                \
        std::cout << "- elapsed time: " << elapsed_time << " seconds" << std::endl; \
    } while (0)

// 定义链表节点
class ListNode : public boost::intrusive::slist_base_hook<>
{
public:
    int val;
    ListNode(int x) : val(x) {}
};

// 定义链表类型
using List = boost::intrusive::slist<ListNode>;

