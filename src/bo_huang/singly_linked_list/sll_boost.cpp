#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <forward_list>
#include <fstream>
#include <typeinfo>
#include <type_traits>
#include <list>
#include <boost/container/slist.hpp>

#define MEASURE_TIME_HIGH_RESOLUTION(loop_code)                                                                     \
    do                                                                                                              \
    {                                                                                                               \
        auto start_emplace = std::chrono::high_resolution_clock::now();                                             \
        loop_code;                                                                                                  \
        auto end_emplace = std::chrono::high_resolution_clock::now();                                               \
        auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace); \
        std::cout << "- elapsed time: " << duration_emplace.count() << " microseconds" << std::endl;                \
        times.push_back(duration_emplace.count());                                                                  \
    } while (0)

// Define the linked list node
class ListNode : public boost::intrusive::slist_base_hook<>
{
public:
    int val;
    ListNode(int x) : val(x) {}
};

// Define the linked list type
using List = boost::intrusive::slist<ListNode>;

template <typename T>
void test_boost_slist(int loop_num, std::vector<double> &times)
{

    List *arr = new List();

    std::cout << "boost::unordered_map emplace ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; i++) {
            ListNode *node_ptr = new ListNode{i};
            arr->push_front(*node_ptr);
        });

    int value = 0;
    std::cout << "boost::unordered_map read ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto node
             : *arr) {
            value = node.val;
        });

    std::cout << "boost::slist erase ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = arr->begin(); it != arr->end(); ) {
            it = arr->erase(it);
        });

    delete arr;

}

int main()
{
    std::vector<double> times;
    // int loop_num = 1000;

    const int kNumTests = 4;
    constexpr int kNumIters[kNumTests] = {1000, 10000, 100000, 1000000};
    std::ofstream file("../../../data/boost_results.csv");

    // test
    int row = 0;
    // table header
    file << "container,datatype,operation,loop_num,time(us)" << std::endl;
    for (int i = 0; i < kNumTests; i++)
    {
        int loop_num = kNumIters[i];
        // test_std_forward_list<int>(loop_num);
        // }

        // 测试 std::forward_list
        // std::cout << "std::forward_list<int> test:" << std::endl;
        test_boost_slist<int>(loop_num, times);
        test_boost_slist<double>(loop_num, times);
        test_boost_slist<std::vector<int>>(loop_num, times);

        std::string container_names[1] = {"boost::intrusive::slist"};
        std::string datatype_names[3] = {typeid(int).name(), typeid(double).name(), typeid(std::vector<int>).name()};
        std::string operation_names[3] = {"push_front", "read", "erase"};

        for (int container_num = 0; container_num < 1; container_num++)
        {
            for (int datatype_num = 0; datatype_num < 3; datatype_num++)
            {
                for (int op_num = 0; op_num < 3; op_num++)
                {
                    file << container_names[container_num] << "," << datatype_names[datatype_num] << "," << operation_names[op_num] << ",";
                    file << std::to_string(loop_num) << "," << times[row++] << std::endl;
                }
            }
        }
    }

    file.close();

    return 0;
}
