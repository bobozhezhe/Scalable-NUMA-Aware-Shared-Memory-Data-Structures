#include <iostream>
#include <mutex>

template <typename T>
class Node {
public:
    T data;
    Node<T>* next;
    std::mutex mtx;

    Node(T value) : data(value), next(nullptr) {}

    ~Node() {
        delete next;
    }
};

template <typename T>
class LinkedList {
public:
    LinkedList() : head(nullptr) {}

    void add(T value) {
        Node<T>* newNode = new Node<T>(value);
        std::lock_guard<std::mutex> lock(mtx);
        newNode->next = head;
        head = newNode;
    }

    void remove(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        Node<T>* curr = head;
        Node<T>* prev = nullptr;

        while (curr != nullptr && curr->data != value) {
            prev = curr;
            curr = curr->next;
        }

        if (curr != nullptr) {
            if (prev != nullptr) {
                prev->next = curr->next;
            } else {
                head = curr->next;
            }
            curr->next = nullptr;
            delete curr;
        }
    }

    void print() {
        std::lock_guard<std::mutex> lock(mtx);
        Node<T>* curr = head;
        while (curr != nullptr) {
            std::cout << curr->data << " ";
            curr = curr->next;
        }
        std::cout << std::endl;
    }

private:
    Node<T>* head;
    std::mutex mtx;
};
