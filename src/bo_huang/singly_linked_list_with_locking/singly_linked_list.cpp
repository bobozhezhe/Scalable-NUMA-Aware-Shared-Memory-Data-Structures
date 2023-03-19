#include <iostream>

using namespace std;

// Definition of a node in the linked list
struct Node {
    int data;
    Node* next;
};

// Class for the singly linked list
class LinkedList {
private:
    Node* head; // Pointer to the head node of the linked list

public:
    LinkedList() {
        head = nullptr; // Initialize the head pointer to null
    }

    // Function to add a new node at the beginning of the linked list
    void addNode(int val) {
        Node* newNode = new Node; // Create a new node
        newNode->data = val; // Set the data of the new node
        newNode->next = head; // Make the new node point to the current head node
        head = newNode; // Update the head pointer to point to the new node
    }

    // Function to print the linked list
    void printList() {
        Node* curr = head; // Start at the head node
        while (curr != nullptr) { // Continue until the end of the list
            cout << curr->data << " "; // Print the current node's data
            curr = curr->next; // Move to the next node
        }
        cout << endl; // Print a new line at the end
    }
};

int main() {
    LinkedList list; // Create a new linked list

    // Add some nodes to the list
    list.addNode(3);
    list.addNode(7);
    list.addNode(1);
    list.addNode(9);

    // Print the list
    list.printList();

    return 0;
}