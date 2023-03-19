#include <iostream>
#include <thread>
#include <chrono>
#include "../lib/lock_free_queue.h"

const int NUM_THREADS = 4;
const int NUM_ENQUEUE = 1000000;

void enqueue_worker(lock_free_queue<int>& queue, int start_value) {
    for (int i = 0; i < NUM_ENQUEUE; ++i) {
        queue.enqueue(start_value + i);
    }
}

int main() {
    lock_free_queue<int> queue;
    std::thread threads[NUM_THREADS];
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = std::thread(enqueue_worker, std::ref(queue), i * NUM_ENQUEUE);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Enqueued " << (NUM_THREADS * NUM_ENQUEUE) << " items in " << duration.count() << " ms" << std::endl;
    return 0;
}
