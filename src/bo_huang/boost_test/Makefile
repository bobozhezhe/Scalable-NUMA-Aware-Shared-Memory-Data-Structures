test:
	g++ test_numa_memcpy.cpp -o test_numa_memcpy -I/usr/lib -lpthread -lrt -lnuma
test_memcpy:
	g++ test_memcpy.cpp -o test_memcpy -I/usr/lib -lboost_thread -lpthread -lrt
test_shared_mem:
	g++ test_shared_mem.cpp -o test_shared_mem -I/usr/lib -lboost_thread -lpthread -lrt
boost:
	g++ test_interprocess.cpp -o test_interprocess -I/usr/lib -lboost_thread -lpthread -lrt
simple:
	g++ test_boost.cpp -o test_boost -I/usr/lib -lboost_thread -lpthread
clean:
	rm -rf test_boost test_interprocess test_memcpy test_shared_mem test_numa_memcpy
