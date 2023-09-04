CC=gcc
OUTPUTDIR=./bin
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Werror -g -ggdb -Wextra -std=c11
SRC=./src/*.c
LIBS=-lncurses
all: main

main:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUTPUTDIR)/main $(SRC) $(LIBS)

run:
	$(OUTPUTDIR)/main

clean:
	rm $(OUTPUTDIR)/main
