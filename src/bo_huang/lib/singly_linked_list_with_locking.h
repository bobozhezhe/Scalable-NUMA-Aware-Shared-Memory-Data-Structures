#ifndef SINGLY_LINKED_LIST_WITH_LOCKING_H
#define SINGLY_LINKED_LIST_WITH_LOCKING_H

#include <iostream>
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


#endif
