CC = mpic++

# Flags
CFLAGS = -std=c++17 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
BOOST_LDFLAGS = -lboost_serialization -lboost_mpi -lrt

all: test test_boost_lock_free_queue test_numa_lock_free_queue test_lock_free_queue multi_thread_lfq

test : test.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS) $(LDFLAGS)

multi_thread_lfq : multi_thread_lfq.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS) $(LDFLAGS)

test_boost_lock_free_queue : test_boost_lock_free_queue.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS) $(LDFLAGS)

test_numa_lock_free_queue : test_numa_lock_free_queue.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_lock_free_queue : test_lock_free_queue.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o test_lock_free_queue test_numa_lock_free_queue test_boost_lock_free_queue
	rm -rf multi_thread_lfq
	rm -rf test
