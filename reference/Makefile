# Compiler
GCC = g++
CC = mpic++

# Flags
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
BOOST_LDFLAGS = -lboost_serialization -lboost_mpi -lrt

all: example2

example2: example2.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

test_numa_umap: test_numa_umap.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f example2
