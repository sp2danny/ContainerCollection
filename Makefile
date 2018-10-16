
CC = clang++
CFLAGS = -std=c++17 -Wall -Wextra -Werror
OPT = -O3 -DNDEBUG
#OPT = -O0 -g -D_DEBUG -DFULL_DIAG

compile: test.out

clean:
	rm bin/*.o test.out

all: clean compile

bin/test_all.o: src/test_all.cpp src/inline_vector.hpp src/test_item.hpp src/container_tester.hpp src/container_operations.hpp src/splice_list.hpp
	$(CC) $(CFLAGS) $(OPT) -c src/test_all.cpp -o bin/test_all.o


test.out: bin/test_all.o
	$(CC) $(OPT) bin/test_all.o -o test.out
