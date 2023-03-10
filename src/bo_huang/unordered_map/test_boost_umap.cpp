#include <boost/mpi.hpp>
#include <boost/unordered_map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>

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
    std::unordered_map<Key, T> data_;
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

int main(int argc, char** argv) {
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;
    int rank = comm.rank();

    const int TIMES = 1000000;
    const int NUM_PROCESSES = 4;

    mpi_unordered_map<int, int> a;

    double start_time = MPI_Wtime();
    // Distribute emplace operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        a.emplace(i, i);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);
    if (rank == 0) {
        std::cout << "Emplace elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    // boost::mpi::barrier(comm);
    boost::mpi::all_reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());


    start_time = MPI_Wtime();
    // Distribute get operations across all processes
    for (int i = rank; i < TIMES; i += NUM_PROCESSES) {
        int value = a.get(i);
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0);

    if (rank == 0) {
        std::cout << "Getting key elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    return 0;
}
