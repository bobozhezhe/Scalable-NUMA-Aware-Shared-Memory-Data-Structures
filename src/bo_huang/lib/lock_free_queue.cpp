#include <atomic>

template <typename T>
lock_free_queue<T>::lock_free_queue() : head(new Node(T())), tail(head.load()) {}

template <typename T>
lock_free_queue<T>::~lock_free_queue()
{
    while (Node *old_head = head.load())
    {
        head.store(old_head->next);
        delete old_head;
    }
}

template <typename T>
void lock_free_queue<T>::enqueue(const T &data)
{
    Node *new_node = new Node(data);
    Node *old_tail = tail.load();
    // while (!old_tail->next.compare_exchange_weak((Node*)nullptr, new_node)) {
    //     old_tail = tail.load();
    // }
    while (!tail.compare_exchange_weak(old_tail, new_node))
        ;
}

template <typename T>
bool lock_free_queue<T>::dequeue(T &result)
{
    Node *old_head = head.load();
    while (old_head != tail.load())
    {
        if (head.compare_exchange_weak(old_head, old_head->next))
        {
            result = old_head->next->data;
            delete old_head;
            return true;
        }
        old_head = head.load();
    }
    return false;
}
