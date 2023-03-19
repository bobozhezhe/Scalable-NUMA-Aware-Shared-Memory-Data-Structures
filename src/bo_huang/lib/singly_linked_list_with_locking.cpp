#include "singly_linked_list_with_locking.h"
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
void SinglyLinkedListWithLocking<T>::print() const
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

