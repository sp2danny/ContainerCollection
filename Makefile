
CC = clang++
CFLAGS = -std=c++17 -Wall -Wextra -Werror
#OPT = -O3 -DNDEBUG
OPT = -O0 -g -D_DEBUG 
#-DFULL_DIAG

compile: test.out

clean:
	rm bin/*.o *.out

all: clean compile

bin/test_all.o: src/test_all.cpp src/inline_vector.hpp src/test_item.hpp src/container_tester.hpp src/container_operations.hpp src/splice_list.hpp src/avl_vector.hpp
	$(CC) $(CFLAGS) $(OPT) -c src/test_all.cpp -o bin/test_all.o


bin/test_tv.o: src/test_tv.cpp src/avl_vector.hpp src/hb_tree.hpp
	$(CC) $(CFLAGS) $(OPT) -c src/test_tv.cpp -o bin/test_tv.o

test.out: bin/test_all.o
	$(CC) $(OPT) bin/test_all.o -o test.out

tv.out: bin/test_tv.o
	$(CC) $(OPT) bin/test_tv.o -o tv.out

