CC=gcc
TEST_FILE=./src/test.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Werror -g -ggdb -Wextra -std=c11
SRC=./src/*.c
LIBS=-lncurses
all: main

main:
	$(CC) $(CFLAGS) $(INCLUDE) -o mi $(SRC) $(LIBS)

run:
	./mi $(TEST_FILE)
	cat $(TEST_FILE)

debug:
	valgrind ./mi $(TEST_FILE)

clean:
	rm ./mi
