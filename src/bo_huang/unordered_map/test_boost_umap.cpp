#include <boost/mpi.hpp>
#include <boost/unordered_map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
// #include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <vector>

/*
template<typename Key, typename T>
class mpi_unordered_map {
public:
    mpi_unordered_map() {}

    template<typename ...Args>
    void emplace(Args&& ...args) {
        data_.emplace(std::forward<Args>(args)...);
    }

    T& get(const Key& key) {
        auto iter = data_.find(key);
        if (iter != data_.end()) {
            return iter->second;
        }
        throw std::out_of_range("key not found");
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & data_;
    }

private:
    boost::unordered_map<Key, T> data_;
};

namespace boost {
namespace serialization {

template<typename Key, typename T>
struct version<mpi_unordered_map<Key, T>> {
    BOOST_STATIC_CONSTANT(int, value = 1);
};

template<typename Archive, typename Key, typename T>
void serialize(Archive& ar, mpi_unordered_map<Key, T>& obj, const unsigned int version) {
    obj.serialize(ar, version);
}

} // namespace serialization
} // namespace boost
*/

using namespace boost::interprocess;
using namespace boost::mpi;

int main(int argc, char** argv) {

    const int TIMES = 1000000;
    const int NUM_PROCESSES = 4;
    const int MEM_LENGTH = 65536;
    const int MAP_LENGTH = 1024;

    // Initialize MPI
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    int rank = comm.rank();
    // Define the shared memory name
    std::string shm_name = "my_shared_memory";
    // Create or open the shared memory segment
    shared_memory_object::remove(shm_name.c_str());
    boost::interprocess::managed_shared_memory segment(open_or_create, shm_name.c_str(), MEM_LENGTH);

    // Define an allocator for the unordered_map
    typedef boost::interprocess::allocator<std::pair<const int, int>, managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the unordered_map
    typedef boost::unordered_map<int, int, std::hash<int>, std::equal_to<int>, ShmemAllocator> MyHashMap;

    // Construct the unordered_map in the shared memory
    MyHashMap* map = segment.construct<MyHashMap>("my_map")(segment.get_segment_manager());


    // mpi_unordered_map<int, int> a;

    double start_time = MPI_Wtime();
    // Distribute emplace operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        // a.emplace(i, i);
        (*map)[i % MAP_LENGTH] = i;
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;

    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);
    // Synchronize the shared memory segment
    // segment.shmem_barrier::operator()(comm);


    if (comm.rank() == 0) {
        std::cout << "Rank 0: Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (comm.rank() == 1) {
        std::cout << "Rank 1: Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());


    start_time = MPI_Wtime();
    // Distribute get operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        int value = (*map)[i % MAP_LENGTH];

    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (comm.rank() == 0) {
        std::cout << "Rank 0: Getting key elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }
    else if (comm.rank() == 1) {
        std::cout << "Rank 1: Getting key elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }


    // Destroy the unordered_map and the shared memory segment
    segment.destroy<MyHashMap>("my_map");
    shared_memory_object::remove(shm_name.c_str());

    return 0;
}