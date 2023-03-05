

CC = mpic++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3 -fopenmp
# -Werror 
LDFLAGS = -I/usr/lib -lpthread -lrt -lnuma
# LDFLAGS = -lnuma

all: test_numa_umap

test: test.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test_numa_umap: test_numa_umap.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f test_numa_umap test
