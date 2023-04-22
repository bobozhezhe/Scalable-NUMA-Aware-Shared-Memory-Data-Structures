
#include <iostream>
#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <chrono>

const int EMPLACE_NUM = 1000000;
const int AT_NUM = 1000000;

void test_boost_unordered_map() {

    std::cout << "Emplace() ops : " << EMPLACE_NUM << " , At() ops : " << AT_NUM << std::endl;

    // 初始化unordered_map
    boost::unordered_map<int, int> my_map;

    auto start_emplace = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < EMPLACE_NUM; i++) {
        my_map.emplace(i, i);
    }
    auto end_emplace = std::chrono::high_resolution_clock::now();
    auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace);
    std::cout << "boost::unordered_map emplace total time taken: " << duration_emplace.count() << " microseconds" << std::endl;

    // 进行10000次get操作，并记录每次操作所花费的时间
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < AT_NUM; i++) {
        my_map.at(i%EMPLACE_NUM);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "boost::unordered_map total time taken: " << duration.count() << " microseconds" << std::endl;
}

void test_std_unordered_map() {
    // 初始化unordered_map
    std::unordered_map<int, int> my_map;
    auto start_emplace = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < EMPLACE_NUM; i++) {
        my_map.emplace(i, i);
    }
    auto end_emplace = std::chrono::high_resolution_clock::now();
    auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace);
    std::cout << "std::unordered_map emplace total time taken: " << duration_emplace.count() << " microseconds" << std::endl;

    // 进行10000次get操作，并记录每次操作所花费的时间
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < AT_NUM; i++) {
        my_map.at(i%EMPLACE_NUM);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "std::unordered_map total time taken: " << duration.count() << " microseconds" << std::endl;
}

int main() {
    test_boost_unordered_map();
    test_std_unordered_map();
    return 0;
}

