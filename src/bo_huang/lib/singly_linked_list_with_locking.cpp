#include "singly_linked_list_with_locking.h"
#include <iostream>

template <typename T>
singly_linked_list_with_locking<T>::singly_linked_list_with_locking() : head(nullptr) {}

template <typename T>
singly_linked_list_with_locking<T>::~singly_linked_list_with_locking()
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
void singly_linked_list_with_locking<T>::push_back(T value)
{
    Node *node = new Node(value);
    std::lock_guard<std::mutex> lock(std::mutex);
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
void singly_linked_list_with_locking<T>::print() const
{
    std::lock_guard<std::mutex> lock(std::mutex);
    Node *current = head;
    while (current != nullptr)
    {
        std::cout << current->value << " ";
        current = current->next;
    }
    std::cout << std::endl;
}


template <typename T>
bool singly_linked_list_with_locking<T>::contains(T value) const {
    std::lock_guard<std::mutex> lock(std::mutex); // lock the mutex

    Node *current = head;
    while (current != nullptr) {
        if (current->value == value) {
            return true; // found a match
        }
        current = current->next;
    }
    return false; // no match found
}
