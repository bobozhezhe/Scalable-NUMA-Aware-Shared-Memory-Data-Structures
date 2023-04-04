CC = mpic++

# Flags
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
BOOST_LDFLAGS = -lboost_serialization -lboost_mpi -lrt

all: test_numa_memcpy test_memcpy test_shared_mem test_interprocess test_boost

test_numa_memcpy : test_numa_memcpy.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
# g++ test_numa_memcpy.cpp -o test_numa_memcpy -I/usr/lib -lpthread -lrt -lnuma

test_memcpy : test_memcpy.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
# g++ test_memcpy.cpp -o test_memcpy -I/usr/lib -lboost_thread -lpthread -lrt

test_shared_mem : test_shared_mem.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
# g++ test_shared_mem.cpp -o test_shared_mem -I/usr/lib -lboost_thread -lpthread -lrt

test_interprocess : test_interprocess.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
# g++ test_interprocess.cpp -o test_interprocess -I/usr/lib -lboost_thread -lpthread -lrt

test_boost : test_boost.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)
# g++ test_boost.cpp -o test_boost -I/usr/lib -lboost_thread -lpthread

clean:
	rm -rf test_boost test_interprocess test_memcpy test_shared_mem test_numa_memcpy
