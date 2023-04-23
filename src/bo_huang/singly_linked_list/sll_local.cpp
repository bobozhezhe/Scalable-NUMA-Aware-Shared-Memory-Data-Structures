#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <forward_list>

#define MEASURE_TIME_HIGH_RESOLUTION(loop_code)                                                                     \
    do                                                                                                              \
    {                                                                                                               \
        auto start_emplace = std::chrono::high_resolution_clock::now();                                             \
        loop_code;                                                                                                  \
        auto end_emplace = std::chrono::high_resolution_clock::now();                                               \
        auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace); \
        std::cout << "- elapsed time: " << duration_emplace.count() << " microseconds" << std::endl;                \
    } while (0)

template <typename T>
void test_std_forward_list(int loop_num)
{
    // 初始化
    std::forward_list<T> l;
    std::cout << "std::forward_list push_front() with " << loop_num << " elements of type " << typeid(T).name() << std::endl;
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; i++) {
            l.push_front(T(i));
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
            it = l.erase_after(it);
        });
}

int main()
{
    const int kNumTests = 4;
    constexpr int kNumIters[kNumTests] = {1000, 10000, 100000, 1000000};

    // int
    std::cout << "std::forward_list<int> test:" << std::endl;
    for (int i = 0; i < kNumTests; i++) {
        int loop_num = kNumIters[i];
        test_std_forward_list<int>(loop_num);
    }

    // double
    std::cout << "std::forward_list<double> test:" << std::endl;
    for (int i = 0; i < kNumTests; i++) {
        int loop_num = kNumIters[i];
        test_std_forward_list<double>(loop_num);
    }

    // 32-byte vector
    // std::cout << "std::forward_list<std::vector<char, 32>> test:" << std::endl;
    // for (int i = 0; i < kNumTests; i++) {
    //     int loop_num = kNumIters[i];
    //     test_std_forward_list<std::vector<char, 32>>(loop_num);
    // }

    return 0;
}
