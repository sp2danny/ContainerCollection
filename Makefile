
CC = clang++
CFLAGS = -Wall -Wextra -Werror -pedantic

compile: test.out

bin/test_all.o: src/test_all.cpp src/inline_vector.hpp
	$(CC) $(CFLAGS) -c src/test_all.cpp -o bin/test_all.o


test.out: bin/test_all.o
	$(CC) bin/test_all.o -o test.out
