CC=gcc
OUTPUTDIR=./bin
TEST_FILE=./files/test_file.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Werror -g -ggdb -Wextra -std=c11
SRC=./src/*.c
LIBS=-lncurses
all: main

main:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUTPUTDIR)/main $(SRC) $(LIBS)

test:
	$(OUTPUTDIR)/main file && rm file

run:
	$(OUTPUTDIR)/main $(TEST_FILE)
	cat $(TEST_FILE)
debug:
	valgrind $(OUTPUTDIR)/main $(TEST_FILE)
	cat $(TEST_FILE)

clean:
	rm $(OUTPUTDIR)/main
