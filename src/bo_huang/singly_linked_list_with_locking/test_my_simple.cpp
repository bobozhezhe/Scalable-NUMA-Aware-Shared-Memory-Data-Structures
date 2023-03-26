#include <iostream>
#include <thread>
#include <chrono>
#include "../lib/singly_linked_list_with_locking.h"

const int NUM_THREADS = 4;
const int NUM_ELEMENTS = 100;

void test_list(singly_linked_list_with_locking<int>& list, int start, int end)
{
    for (int i = start; i < end; i++) {
        list.push_back(i);
    }
}

int main()
{
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    singly_linked_list_with_locking<int> list;
    std::thread threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i] = std::thread(test_list, std::ref(list), i * (NUM_ELEMENTS / NUM_THREADS), (i + 1) * (NUM_ELEMENTS / NUM_THREADS));
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }

    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

    std::cout << "Elapsed time: " << time_span.count() << " seconds.\n";

    return 0;
}
