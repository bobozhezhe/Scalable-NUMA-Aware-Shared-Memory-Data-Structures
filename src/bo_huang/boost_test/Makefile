test:
	g++ test_numa_memcpy.cpp -o test_numa_memcpy -I/usr/lib -lpthread -lrt -lnuma
test_memcpy:
	g++ test_memcpy.cpp -o test_memcpy -I/usr/lib -lboost_thread -lpthread -lrt
tc_first:
	g++ tc_first.cpp -o tc_first -I/usr/lib -lboost_thread -lpthread -lrt
boost:
	g++ test_boost.cpp -o test_boost -I/usr/lib -lboost_thread -lpthread -lrt
simple:
	g++ test_simplest.c -o test_simplest -I/usr/lib -lboost_thread -lpthread
clean:
	rm -rf test_simplest test_boost test_memcpy tc_first test_numa_memcpy
