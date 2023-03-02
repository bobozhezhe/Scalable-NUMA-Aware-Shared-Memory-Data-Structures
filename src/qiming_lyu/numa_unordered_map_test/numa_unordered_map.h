//
// Created by lqm on 3/1/23.
//

#ifndef UNTITLED1_NUMA_UNORDERED_MAP_H
#define UNTITLED1_NUMA_UNORDERED_MAP_H
#include <iostream>
#include <unordered_map>
#include <numa.h>

template <typename Key, typename Value>
class numa_unordered_map {
public:
    numa_unordered_map() = default;
    void insert(Key key, Value value, int node);
    Value* find(Key key) const;

private:
    std::unordered_map<Key, Value*> map_;
};

#include "numa_unordered_map.cpp"

#endif //UNTITLED1_NUMA_UNORDERED_MAP_H
