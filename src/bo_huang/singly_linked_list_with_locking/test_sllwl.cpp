#include "../lib/singly_linked_list_with_locking.h"
#include <iostream>
#include <thread>


void push_values(SinglyLinkedListWithLocking<int>& list, int start, int end) {
    for (int i = start; i <= end; i++) {
        list.push_back(i);
    }
}

int main() {
    SinglyLinkedListWithLocking<int> list;
    
    // Spawn two threads to push values into the list concurrently
    std::thread t1(push_values, std::ref(list), 1, 5);
    std::thread t2(push_values, std::ref(list), 6, 10);
    
    // Wait for the threads to finish
    t1.join();
    t2.join();
    
    // Print the contents of the list
    list.print();
    
    return 0;
}
