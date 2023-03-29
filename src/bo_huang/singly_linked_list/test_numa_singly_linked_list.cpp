#include <iostream>
#include <chrono>
#include <thread>
#include <numa.h>

using namespace std;

struct Node {
    int data;
    Node* next;
};

class LinkedList {
private:
    Node* head;
public:
    LinkedList() {
        head = nullptr;
    }

    void insert(int x) {
        Node* newNode = new Node;
        newNode->data = x;
        newNode->next = head;
        head = newNode;
    }

    void print() {
        Node* curr = head;
        while (curr != nullptr) {
            cout << curr->data << " ";
            curr = curr->next;
        }
        cout << endl;
    }

    Node* getHead() {
        return head;
    }
};

int main() {
    const int NUM_NODES = numa_num_configured_nodes();
    const int NUM_THREADS = NUM_NODES == 0 ? 1 : NUM_NODES;
    const int NUM_VALUES = 1000000;

    // create linked list
    LinkedList list;
    for (int i = 0; i < NUM_VALUES; i++) {
        list.insert(i);
    }

    // spawn threads to read linked list from different nodes
    thread threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i] = thread([i, &list]() {
            numa_run_on_node(i);
            Node* curr = list.getHead();
            auto start = chrono::high_resolution_clock::now();
            while (curr != nullptr) {
                curr = curr->next;
            }
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
            cout << "Node " << i << " took " << duration << " microseconds\n";
        });
    }

    // join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }

    return 0;
}
