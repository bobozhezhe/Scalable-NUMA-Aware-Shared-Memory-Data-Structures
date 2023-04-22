
#include <iostream>
#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <chrono>
#include "../lib/numa_experience.h"

const int EMPLACE_NUM = 1000000;
const int AT_NUM = 1000000;

void test_boost_unordered_map()
{

    std::cout << "Emplace() ops : " << EMPLACE_NUM << " , At() ops : " << AT_NUM << std::endl;

    // 初始化unordered_map
    boost::unordered_map<int, int> my_map;

    std::cout << "boost::unordered_map emplace ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < EMPLACE_NUM; i++) {
            my_map.emplace(i, i);
        });

    std::cout << "boost::unordered_map at ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < AT_NUM; i++) {
            my_map.at(i % EMPLACE_NUM);
        });
}

void test_std_unordered_map()
{
    // 初始化unordered_map
    std::unordered_map<int, int> my_map;
    std::cout << "std::unordered_map emplace  ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < EMPLACE_NUM; i++) {
            my_map.emplace(i, i);
        });

    std::cout << "std::unordered_map at  ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < AT_NUM; i++) {
            my_map.at(i % EMPLACE_NUM);
        });
}

int main()
{
    test_boost_unordered_map();
    test_std_unordered_map();
    return 0;
}
