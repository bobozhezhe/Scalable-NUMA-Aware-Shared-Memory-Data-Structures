
#include <boost/mpi.hpp>
#include <chrono>
#define MEASURE_TIME_MPI(loop_code)                                                 \
    do                                                                              \
    {                                                                               \
        double max_elapsed_time;                                                    \
        double start_time = MPI_Wtime();                                            \
        loop_code;                                                                  \
        double end_time = MPI_Wtime();                                              \
        double elapsed_time = end_time - start_time;                                \
        boost::mpi::reduce(comm, elapsed_time, max_elapsed_time, boost::mpi::maximum<double>(), 0); \
        std::cout << "- elapsed time: " << max_elapsed_time << " seconds" << std::endl; \
    } while (0)

#define MEASURE_TIME_HIGH_RESOLUTION(loop_code)                                     \
    do                                                                              \
    {                                                                               \
        auto start_emplace = std::chrono::high_resolution_clock::now();             \
        loop_code;                                                                  \
        auto end_emplace = std::chrono::high_resolution_clock::now();               \
        auto duration_emplace = std::chrono::duration_cast<std::chrono::microseconds>(end_emplace - start_emplace); \
        std::cout << "- elapsed time: " << duration_emplace.count() << " microseconds" << std::endl;    \
    } while (0)
