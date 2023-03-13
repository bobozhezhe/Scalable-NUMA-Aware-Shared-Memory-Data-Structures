#include <unordered_set>
#include <vector>
#include <iostream>
#include <boost/serialization/unordered_set.hpp>
#include <boost/mpi.hpp>

class distributed_unordered_set {
public:
    distributed_unordered_set(boost::mpi::communicator comm) : comm_(comm) {}

    void insert(int value) {
        int rank = comm_.rank();
        if (rank == 0) {
            master_set_.insert(value);
            // Broadcast the updated set to all other processes
            boost::mpi::broadcast(comm_, master_set_, 0);
        } else {
            // Receive the updated set from the master process
            boost::mpi::broadcast(comm_, master_set_, 0);
        }
    }

    bool contains(int value) {
        return master_set_.count(value) > 0;
    }

private:
    boost::mpi::communicator comm_;
    std::unordered_set<int> master_set_;
};

int main(int argc, char** argv) {
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    const int TIMES = 1000000;
    const int NUM_PROCESSES = world.size();

    distributed_unordered_set a(world);

    double start_time = MPI_Wtime();
    // Distribute insert operations across all processes
    for (int i = world.rank(); i < TIMES; i += NUM_PROCESSES) {
        a.insert(i);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_elapsed_time;
    boost::mpi::reduce(world, elapsed_time, max_elapsed_time, MPI_MAX, 0);
    if (world.rank() == 0) {
        std::cout << "Insert elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    // boost::mpi::barrier(world);
    boost::mpi::all_reduce(world, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>());

    start_time = MPI_Wtime();
    // Distribute contains operations across all processes
    for (int i = world.rank(); i < TIMES; i += NUM_PROCESSES) {
        bool contains_value = a.contains(i);
    }
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;
    boost::mpi::reduce(world, elapsed_time, max_elapsed_time, MPI_MAX, 0);
    if (world.rank() == 0) {
        std::cout << "Contains elapsed time: " << max_elapsed_time << " seconds" << std::endl;
    }

    return 0;
}
