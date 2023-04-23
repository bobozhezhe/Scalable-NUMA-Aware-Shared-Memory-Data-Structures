#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <forward_list>
#include <fstream>
#include <typeinfo>
#include <type_traits>
#include <list>

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

template <typename T>
void test_std_forward_list(int loop_num, std::vector<double> &times)
{
    // 初始化
    std::forward_list<T> l;
    std::cout << "std::forward_list push_front() with " << loop_num << " elements of type " << typeid(T).name() << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; i++) {
            T value;
            l.push_front(value);
        });

    T value;
    std::cout << "std::forward_list traverse " << loop_num << " times" << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = l.begin(); it != l.end(); ++it) {
            value = *it;
        });

    std::cout << "std::forward_list erase_after() with " << loop_num << " elements" << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = l.before_begin(); it != l.end();) {
            if (std::next(it) == l.end())
            { // 如果 it 后面没有元素，退出循环
                break;
            }
            it = l.erase_after(it);
        });
}

template <typename T>
void test_std_vector(int loop_num, std::vector<double> &times)
{
    // 初始化
    std::vector<T> v;
    std::cout << "std::vector push_back() with " << loop_num << " elements of type " << typeid(T).name() << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; i++) {
            T value;
            v.push_back(value);
        });

    T value;
    std::cout << "std::vector traverse " << loop_num << " times" << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = v.begin(); it != v.end(); ++it) {
            value = *it;
        });

    std::cout << "std::vector pop_back() with " << loop_num << " elements" << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; i++) {
            v.pop_back();
        });
}

template <typename T>
void test_std_list(int loop_num, std::vector<double> &times)
{
    // 初始化
    std::list<T> l;
    std::cout << "std::list push_back() with " << loop_num << " elements of type " << typeid(T).name() << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; i++) {
            T value;
            l.push_back(value);
        });

    T value;
    std::cout << "std::list traverse " << loop_num << " times" << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = l.begin(); it != l.end(); ++it) {
            value = *it;
        });

    std::cout << "std::list erase() with " << loop_num << " elements" << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = l.begin(); it != l.end();) {
            it = l.erase(it);
        });
}

int main()
{
    std::vector<double> times;
    // int loop_num = 1000;

    const int kNumTests = 4;
    constexpr int kNumIters[kNumTests] = {1000, 10000, 100000, 1000000};
    std::ofstream file("local_results.csv");

    // test
    int row = 0;
    // table header
    file << "container,type,operation,loop_num,time(us)" << std::endl;
    for (int i = 0; i < kNumTests; i++)
    {
        int loop_num = kNumIters[i];
        // test_std_forward_list<int>(loop_num);
        // }

        // 测试 std::forward_list
        // std::cout << "std::forward_list<int> test:" << std::endl;
        test_std_forward_list<int>(loop_num, times);
        test_std_forward_list<double>(loop_num, times);
        test_std_forward_list<std::vector<int>>(loop_num, times);

        // 测试 std::vector
        test_std_vector<int>(loop_num, times);
        test_std_vector<double>(loop_num, times);
        test_std_vector<std::vector<int>>(loop_num, times);

        // 测试 std::list
        test_std_list<int>(loop_num, times);
        test_std_list<double>(loop_num, times);
        test_std_list<std::vector<int>>(loop_num, times);

        std::string container_names[3] = {"std::forward_list", "std::vector", "std::list"};
        std::string datatype_names[3] = {typeid(int).name(), typeid(double).name(), typeid(std::vector<int>).name()};
        std::string operation_names[3] = {"push_front/pop_front", "push_back/pop_back", "push_back/erase"};

        for (int container_num = 0; container_num < 3; container_num++)
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
