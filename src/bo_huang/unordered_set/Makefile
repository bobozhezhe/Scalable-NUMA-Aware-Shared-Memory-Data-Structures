# Compiler
CC = mpic++

# Flags
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
BOOST_LDFLAGS = -lboost_serialization -lboost_mpi

all: test_boost_uset

test_boost_uset: test_boost_uset.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

test_numa_uset: test_numa_uset.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f test_numa_uset test_boost_uset
