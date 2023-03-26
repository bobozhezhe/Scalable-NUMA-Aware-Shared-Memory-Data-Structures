#include <boost/intrusive/slist.hpp>
#include <boost/thread.hpp>

// Define the data structure to be stored in the list
struct MyData {
    int value;
    boost::intrusive::slist_member_hook<> hook;
    // other members...
};

// Define a type for the list
typedef boost::intrusive::slist<MyData> MyList;

// Create an instance of the list
MyList my_list;

// Insert an element into the list
MyData new_data = {42};
boost::unique_lock<boost::shared_mutex> lock(my_list.get_lock());
my_list.push_front(new_data);

// Traverse the list and access its elements
for (MyList::iterator it = my_list.begin(); it != my_list.end(); ++it) {
    std::cout << "Value: " << it->value << std::endl;
}
