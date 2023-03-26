CC = mpic++

# Flags
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 

LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma

BOOST_LDFLAGS = -lboost_serialization -lboost_mpi -lrt

all: test

test: test.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_my_boost_sllwl: test_my_boost_sllwl.cpp
	$(CC) $(CFLAGS) $< -o $@ $(BOOST_LDFLAGS)

test_signly_linked_list_with_locking: test_signly_linked_list_with_locking.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_my_simple: test_my_simple.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_my_numa_sllwl: test_my_numa_sllwl.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_my_sllwl: test_my_sllwl.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o test test_signly_linked_list_with_locking \
		test_my_simple test_my_numa_sllwl test_my_sllwl  test_my_boost_sllwl
