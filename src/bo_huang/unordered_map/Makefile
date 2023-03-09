# Compiler
GCC = g++
CC = mpic++

# Flags
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
BOOST_LDFLAGS = -I/usr/lib -lboost_thread 
#-lboost_system -lboost_mpi -lboost_serialization

all: test_boost_umap

test_boost_umap:
	$(GCC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

test: test.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_numa_umap: test_numa_umap.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f test_numa_umap test test_boost_umap
