// #include <boost/interprocess/mutex_family.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace boost::intrusive;

// Define a node type that can be inserted into the list
struct my_node : public slist_base_hook<> {
    int data;
};

// Define a singly linked list type with locking
// typedef slist_impl<my_node, boost::intrusive::mutex_family> MySList;
typedef slist_impl<my_node, boost::interprocess::mutex_family> MySList;

// Function to insert nodes into the list
void insert_nodes(MySList& slist, int start, int end) {
    for (int i = start; i <= end; ++i) {
        my_node* node = new my_node();
        node->data = i;
        slist.push_front(*node);
    }
}

// Function to traverse the list and print its contents
void print_list(const MySList& slist) {
    for (MySList::const_iterator it = slist.begin(); it != slist.end(); ++it) {
        std::cout << it->data << " ";
    }
    std::cout << std::endl;
}

// Function to remove nodes from the list
void remove_nodes(MySList& slist, int start, int end) {
    for (int i = start; i <= end; ++i) {
        MySList::iterator it = slist.begin();
        while (it != slist.end()) {
            if (it->data == i) {
                slist.erase(it++);
                break;
            } else {
                ++it;
            }
        }
    }
}

int main() {
    // Create a singly linked list with locking
    MySList slist;

    // Insert nodes into the list using multiple threads
    boost::thread thread1(insert_nodes, boost::ref(slist), 1, 5);
    boost::thread thread2(insert_nodes, boost::ref(slist), 6, 10);
    thread1.join();
    thread2.join();

    // Traverse the list and print its contents
    print_list(slist);

    // Remove nodes from the list using multiple threads
    boost::thread thread3(remove_nodes, boost::ref(slist), 1, 5);
    boost::thread thread4(remove_nodes, boost::ref(slist), 6, 10);
    thread3.join();
    thread4.join();

    // Traverse the list and print its contents again
    print_list(slist);

    // Clear the list and destroy its nodes
    slist.clear_and_dispose([](my_node* node) { delete node; });

    return 0;
}
