#include <iostream>
#include <numa.h>
#include "numa_unordered_map.h"

void test1();
void test2();

int main() {
    test1();
    test2();
    return 0;
}

//Bind the process to the node 0, and apply for memory at node 0
void test1() {
    int node = 0; //node num

    numa_available(); // Initialize NUMA library

    //Bind the current process to the specified node
    numa_set_strict(1);
    struct bitmask *mask = numa_bitmask_alloc(2);
    numa_bitmask_setbit(mask, node);
    numa_set_membind(mask);

    numa_unordered_map<std::string, int> my_map;
    my_map.insert("key1", 10, node);
    my_map.insert("key2", 20, node);

    int* value_ptr1 = my_map.find("key1");
    int* value_ptr2 = my_map.find("key2");

    std::cout << "Value of key1: " << *value_ptr1 << std::endl;
    std::cout << "Value of key2: " << *value_ptr2 << std::endl;

    numa_free(value_ptr1, sizeof(int));
    numa_free(value_ptr2, sizeof(int));
}

//Bind the process to the node 0, and apply for memory at node 1
void test2() {
    int node_0 = 0; //node num
    int node_1 = 1; //node num

    numa_available(); // Initialize NUMA library

    // Bind the current process to the specified node
    numa_set_strict(1);
    struct bitmask *mask = numa_bitmask_alloc(2);
    numa_bitmask_setbit(mask, node_0);
    numa_set_membind(mask);

    numa_unordered_map<std::string, int> my_map;
    my_map.insert("key1", 10, node_1);
    my_map.insert("key2", 20, node_1);

    int* value_ptr1 = my_map.find("key1");
    int* value_ptr2 = my_map.find("key2");

    std::cout << "Value of key1: " << *value_ptr1 << std::endl;
    std::cout << "Value of key2: " << *value_ptr2 << std::endl;

    numa_free(value_ptr1, sizeof(int));
    numa_free(value_ptr2, sizeof(int));
}