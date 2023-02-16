#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <cstring>
#include <chrono>
#include <numa.h>

using namespace boost::interprocess;

int main()
{
    const int MEM_LENGTH = 1024;
    const int COPY_TIMES = 1000000;

    try {
        // Create a shared memory object with the name "MySharedMemory" and a size of 1000 bytes
        shared_memory_object shm_obj1(open_or_create, "MySharedMemory1", read_write);
        shm_obj1.truncate(MEM_LENGTH);

        // Map the first shared memory object into our process's address space
        mapped_region region1(shm_obj1, read_write);

        // Get a pointer to the start of the first shared memory region
        char* data1 = static_cast<char*>(region1.get_address());

        // Initialize the first shared memory region with some data
        std::memset(data1, 'A', MEM_LENGTH);

        //// Create another shared memory object with the name "MySharedMemory2" and a size of 1000 bytes
        //shared_memory_object shm_obj2(open_or_create, "MySharedMemory2", read_write);
        //shm_obj2.truncate(MEM_LENGTH);

        //// Map the second shared memory object into our process's address space
        //mapped_region region2(shm_obj2, read_write);

        //// Get a pointer to the start of the second shared memory region
        //char* data2 = static_cast<char*>(region2.get_address());

        // Allocate 1K bytes of memory on NUMA node 0
        void* data2 = numa_alloc_onnode(MEM_LENGTH, 0);

        // Measure the time it takes to perform 1000 memcpy operations
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < COPY_TIMES; i++) {
            std::memcpy(data2, data1, MEM_LENGTH);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Read the string back from the second shared memory region and print it to the console
        std::string result((char*)data2);
        std::cout << "Read from shared memory: " << result << std::endl;
        std::cout << "Time taken: " << duration << " ms" << std::endl;

        // Clean up the shared memory objects
        shared_memory_object::remove("MySharedMemory1");
        // shared_memory_object::remove("MySharedMemory2");

        // Free the allocated memory
        numa_free(data2, MEM_LENGTH);
    }
    catch (interprocess_exception& ex) {
        std::cerr << "Caught exception: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}