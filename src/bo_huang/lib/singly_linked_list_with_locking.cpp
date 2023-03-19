// #include "singly_linked_list_with_locking.h"
#include <iostream>

// SinglyLinkedListWithLocking::SinglyLinkedListWithLocking() : head(nullptr) {}

// SinglyLinkedListWithLocking::~SinglyLinkedListWithLocking() {
//     Node* current = head;
//     while (current != nullptr) {
//         Node* next = current->next;
//         delete current;
//         current = next;
//     }
// }

// void SinglyLinkedListWithLocking::push_back(int value) {
//     std::lock_guard<std::mutex> lock(mutex);
//     Node* new_node = new Node(value);
//     if (head == nullptr) {
//         head = new_node;
//     } else {
//         Node* current = head;
//         while (current->next != nullptr) {
//             current = current->next;
//         }
//         current->next = new_node;
//     }
// }

// void SinglyLinkedListWithLocking::print() const {
//     std::lock_guard<std::mutex> lock(std::mutex);
//     Node* current = head;
//     while (current != nullptr) {
//         std::cout << current->value << " ";
//         current = current->next;
//     }
//     std::cout << std::endl;
// }

#ifndef SINGLY_LINKED_LIST_WITH_LOCKING_H
#define SINGLY_LINKED_LIST_WITH_LOCKING_H

#include <mutex>

template <typename T>
class SinglyLinkedListWithLocking {
public:
    SinglyLinkedListWithLocking();
    ~SinglyLinkedListWithLocking();
    void push_back(T value);
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

template <typename T>
SinglyLinkedListWithLocking<T>::SinglyLinkedListWithLocking() : head(nullptr) {}

template <typename T>
SinglyLinkedListWithLocking<T>::~SinglyLinkedListWithLocking()
{
    Node *current = head;
    while (current != nullptr)
    {
        Node *next = current->next;
        delete current;
        current = next;
    }
}

template <typename T>
void SinglyLinkedListWithLocking<T>::push_back(T value)
{
    Node *node = new Node(value);
    std::lock_guard<std::mutex> lock(mutex);
    // std::lock_guardstd::mutex lock(mutex);
    if (head == nullptr)
    {
        head = node;
    }
    else
    {
        Node *current = head;
        while (current->next != nullptr)
        {
            current = current->next;
        }
        current->next = node;
    }
}

template <typename T>
void SinglyLinkedListWithLocking<T>::print() const
{
    std::lock_guard<std::mutex> lock(mutex);
    // std::lock_guardstd::mutex lock(mutex);
    Node *current = head;
    while (current != nullptr)
    {
        std::cout << current->value << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

#endif

