#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H


template <typename T>
class lock_free_queue {
public:
    lock_free_queue();
    ~lock_free_queue();
    void enqueue(const T& data);
    bool dequeue(T& result);

private:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node(const T& data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

};

#include "lock_free_queue.cpp"

#endif
