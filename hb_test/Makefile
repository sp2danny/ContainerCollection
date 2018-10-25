
CC = clang -std=c11 -Wall -Wextra -Wpedantic -Werror -ferror-limit=3 -static 
LINK = -s 
CFLAGS = -DNDEBUG -O3 

compile: test.out

clean:
	rm *.o

dict.o: dict.c dict.h dict_private.h
	$(CC) $(CFLAGS) -c dict.c

hb_tree.o: hb_tree.c hb_tree.h dict_private.h tree_common.h dict.h
	$(CC) $(CFLAGS) -c hb_tree.c

main.o: main.c dict.h hb_tree.h
	$(CC) $(CFLAGS) -c main.c

tree_common.o: tree_common.c tree_common.h
	$(CC) $(CFLAGS) -c tree_common.c

test.out: dict.o hb_tree.o main.o tree_common.o
	$(CC) $(CFLAGS) dict.o hb_tree.o main.o tree_common.o -o test.out $(LINK)
