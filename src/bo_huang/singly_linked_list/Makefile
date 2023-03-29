CC = mpic++

# Flags
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma

BOOST_LDFLAGS = -lboost_serialization -lboost_mpi -lrt

all: test_boost_singly_linked_list

test_boost_singly_linked_list: test_boost_singly_linked_list.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

test_numa_singly_linked_list: test_numa_singly_linked_list.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o test_numa_singly_linked_list test_boost_singly_linked_list 
