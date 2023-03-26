#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <thread>

int main()
{
    // Create a lock-free queue with a capacity of 10 elements
    boost::lockfree::queue<int> q(10);

    // Create a producer thread that adds numbers 0 to 9 to the queue
    std::thread producer([&q]() {
        for (int i = 0; i < 10; i++) {
            while (!q.push(i)) {} // Keep trying to push until successful
        }
    });

    // Create a consumer thread that retrieves numbers from the queue
    std::thread consumer([&q]() {
        int val;
        for (int i = 0; i < 10; i++) {
            while (!q.pop(val)) {} // Keep trying to pop until successful
            std::cout << val << std::endl;
        }
    });

    // Join the threads and wait for them to finish
    producer.join();
    consumer.join();

    return 0;
}
