#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

using namespace boost::interprocess;

template <typename T>
struct Node {
    T value;
    Node<T>* next;
    mutable boost::mutex mutex;

    Node(const T& val) : value(val), next(nullptr) {}

    void lock() const {
        mutex.lock();
    }

    void unlock() const {
        mutex.unlock();
    }
};

template <typename T, typename Allocator = boost::interprocess::allocator<Node<T>, boost::interprocess::managed_shared_memory::segment_manager>>
class LinkedList {
public:
    typedef Node<T> NodeT;
    typedef typename Allocator::template rebind<NodeT>::other NodeAllocator;

    LinkedList(const char* name, size_t max_size) : allocator_(segment_.get_segment_manager()), head_(nullptr), name_(name) {
        bool created;
        segment_ = managed_shared_memory(open_or_create, name, max_size, 0, &created);
        if (created) {
            head_ = allocator_.allocate(1);
            new (head_) NodeT(T());
        }
        else {
            head_ = segment_.find<NodeT>("head").first;
        }
    }

    void push_front(const T& val) {
        NodeT* new_node = allocator_.allocate(1);
        new (new_node) NodeT(val);

        // Lock the head and new node
        boost::lock_guard<NodeT> head_lock(head_->mutex);
        boost::lock_guard<NodeT> new_node_lock(new_node->mutex);

        new_node->next = head_->next;
        head_->next = new_node;
    }

    void pop_front() {
        if (!head_->next) {
            throw std::runtime_error("List is empty");
        }

        NodeT* to_delete = head_->next;
        head_->next = to_delete->next;

        // Unlock and deallocate the deleted node
        to_delete->~NodeT();
        allocator_.deallocate(to_delete, 1);
    }

    void clear() {
        // Lock the head and the first node
        boost::lock_guard<NodeT> head_lock(head_->mutex);
        if (!head_->next) {
            return;
        }

        boost::lock_guard<NodeT> first_node_lock(head_->next->mutex);

        // Delete all nodes except the head
        NodeT* current = head_->next;
        while (current) {
            NodeT* to_delete = current;
            current = current->next;
            to_delete->~NodeT();
            allocator_.deallocate(to_delete, 1);
        }

        // Reset the head's next pointer
        head_->next = nullptr;
    }

    ~LinkedList() {
        clear();
        segment_.destroy<NodeT>("head");
        segment_.destroy_allocator<NodeT>(allocator_);
        shared_memory_object::remove(name_.c_str());
    }

private:
    boost::interprocess::managed_shared_memory segment_;
    NodeAllocator allocator_;
    NodeT* head_;
    std::string name_;
};
