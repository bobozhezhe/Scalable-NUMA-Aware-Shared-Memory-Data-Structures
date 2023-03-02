//
// Created by lqm on 3/1/23.
//
#include "numa_unordered_map.h"

template <typename Key, typename Value>
void numa_unordered_map<Key, Value>::insert(Key key, Value value, int node) {
    Value* ptr = (Value*)numa_alloc_onnode(sizeof(Value), node);
    *ptr = value;
    map_[key] = ptr;
}

template <typename Key, typename Value>
Value* numa_unordered_map<Key, Value>::find(Key key) const {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return nullptr;
    } else {
        return it->second;
    }
}