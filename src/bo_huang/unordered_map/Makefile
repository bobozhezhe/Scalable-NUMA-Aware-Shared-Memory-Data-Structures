# Compiler
GCC = g++
CC = mpic++

# Flags
CFLAGS = -std=c++17 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
BOOST_LDFLAGS = -lboost_serialization -lboost_mpi -lrt

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
BINS := $(SRCS:.cpp=)


all: $(BINS)

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS) $(LDFLAGS)

clean:
	rm -f $(BINS)


# local_vs_shared: local_vs_shared.cpp
# 	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

# std_vs_boost: std_vs_boost.cpp
# 	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

# test_boost_umap: test_boost_umap.cpp
# 	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

# test: test.cpp
# 	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# test_numa_umap: test_numa_umap.cpp
# 	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# clean:
# 	rm -f test_numa_umap test test_boost_umap std_vs_boost std_vs_boost
