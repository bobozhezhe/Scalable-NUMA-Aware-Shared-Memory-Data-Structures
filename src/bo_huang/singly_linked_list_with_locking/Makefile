all: test_sllwl


	# singly_linked_list_with_locking.cpp 
test_sllwl:
	g++ -c test_sllwl.cpp 
	g++ -std=c++11 test_sllwl.o -o test_sllwl -lpthread

clean:
	rm -f *.o test_sllwl
