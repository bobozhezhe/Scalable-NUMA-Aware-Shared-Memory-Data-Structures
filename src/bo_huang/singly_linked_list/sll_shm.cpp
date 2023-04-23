#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <forward_list>
#include <fstream>
#include <typeinfo>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/container/slist.hpp>
#include <boost/mpi.hpp>
namespace bip = boost::interprocess;

const int MEM_LENGTH = 256 * (1 << 20);

#define MEASURE_TIME_HIGH_RESOLUTION(loop_code)                                                                     \
    do                                                                                                              \
    {                                                                                                               \
        auto start_emplace = std::chrono::high_resolution_clock::now();                                             \
        loop_code;                                                                                                  \
        auto end_emplace = std::chrono::high_resolution_clock::now();                                               \
        auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace); \
        std::cout << "- elapsed time: " << duration_emplace.count() << " microseconds" << std::endl;                \
        times.push_back(duration_emplace.count());                                                                  \
    } while (0)


template <typename T>
void test_shm_slist(int loop_num, std::vector<double> &times)
{
    typedef bip::allocator<int, bip::managed_shared_memory::segment_manager> ShmemAllocator;
    typedef boost::container::slist<int, ShmemAllocator> MySlist;
    std::cout << "datatype = " << typeid(T).name() << std::endl;

    std::string shm_name = "my_shared_memory";
    bip::shared_memory_object::remove(shm_name.c_str());

    bip::managed_shared_memory segment(bip::create_only, shm_name.c_str(), MEM_LENGTH);
    MySlist *slist = segment.construct<MySlist>("MySlist")(segment.get_segment_manager());

    // Insertion performance test
    std::cout << "boost::container::slist on shared_memory push_front ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (int i = 0; i < loop_num; ++i) {
            slist->push_front(i);
        }
    );

    // Read performance test
    std::cout << "boost::container::slist on shared_memory read ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = slist->begin(); it != slist->end(); ++it) {
            int value = *it;
        }
    );

    // Deletion performance test
    std::cout << "boost::container::slist on shared_memory erase ";
    MEASURE_TIME_HIGH_RESOLUTION(
        for (auto it = slist->begin(); it != slist->end(); ) {
            it = slist->erase(it);
        }
    );

    segment.destroy<MySlist>("MySlist");
}


int main()
{
    std::vector<double> times;
    // int loop_num = 1000;

    const int kNumTests = 4;
    constexpr int kNumIters[kNumTests] = {1000, 10000, 100000, 1000000};
    std::ofstream file("shm_results.csv");

    // test
    int row = 0;
    // table header
    file << "container,datatype,operation,loop_num,time(us)" << std::endl;
    for (int i = 0; i < kNumTests; i++)
    {
        int loop_num = kNumIters[i];
        std::cout << "boost::container::slist on shared_memory loop = " << loop_num << std::endl;

        test_shm_slist <int>(loop_num, times);
        test_shm_slist <double>(loop_num, times);
        test_shm_slist <std::vector<int>>(loop_num, times);

        std::string container_names[1] = {"boost::intrusive::slist"};
        std::string datatype_names[3] = {typeid(int).name(), typeid(double).name(), typeid(std::vector<int>).name()};
        std::string operation_names[3] = {"insert", "read", "erase"};

        for (int container_num = 0; container_num < 1; container_num++)
        {
            for (int datatype_num = 0; datatype_num < 3; datatype_num++)
            {
                for (int op_num = 0; op_num < 3; op_num++)
                {
                    file << container_names[container_num] << "," << datatype_names[datatype_num] << "," << operation_names[op_num] << ",";
                    file << std::to_string(loop_num) << "," << times[row++] << std::endl;
                }
            }
        }
    }

    file.close();

    return 0;
}
