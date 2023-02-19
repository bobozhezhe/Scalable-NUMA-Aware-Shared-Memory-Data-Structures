#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
    try {
        // Create a shared memory object with the name "MySharedMemory" and a size of 1000 bytes
        shared_memory_object shm_obj(open_or_create, "MySharedMemory", read_write);
        shm_obj.truncate(1000);

        // Map the shared memory object into our process's address space
        mapped_region region(shm_obj, read_write);

        // Get a pointer to the start of the shared memory region
        char* data = static_cast<char*>(region.get_address());

        // Write a string to the shared memory region
        std::string message = "Hello, shared memory!";
        std::copy(message.begin(), message.end(), data);

        // Read the string back from the shared memory region and print it to the console
        std::string result(data, data + message.size());
        std::cout << "Read from shared memory: " << result << std::endl;

        // Clean up the shared memory object
        shared_memory_object::remove("MySharedMemory");
    }
    catch (interprocess_exception& ex) {
        std::cerr << "Caught exception: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

