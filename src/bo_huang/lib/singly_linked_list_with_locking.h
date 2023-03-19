#ifndef SINGLY_LINKED_LIST_WITH_LOCKING_H
#define SINGLY_LINKED_LIST_WITH_LOCKING_H

#include <iostream>
#include <mutex>

template <typename T>
class singly_linked_list_with_locking {
public:
    singly_linked_list_with_locking();
    ~singly_linked_list_with_locking();
    void push_back(T value);
    bool contains(T value) const;
    void print() const;

private:
    struct Node
    {
        T value;
        Node *next;
        Node(T value) : value(value), next(nullptr) {}
    };
    Node *head;
    std::mutex mutex;
};

#include "singly_linked_list_with_locking.cpp"

#endif
