CC=gcc
OUT_DIR=./bin
OUT_FILE=$(OUT_DIR)/mi
TEST_FILE=./src/test.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -g -ggdb -Wextra -std=c11 -O3
SRC=./src/*.c
LIBS=-lncursesw -lm -pthread

all: main
main:
	mkdir -p $(OUT_DIR) ; $(CC) $(CFLAGS) $(INCLUDE) -o $(OUT_FILE) $(SRC) $(LIBS)

run:
	$(OUT_FILE) .

debug:
	valgrind $(OUT_FILE) $(TEST_FILE)

clean:
	rm $(OUT_FILE) $(TEST_FILE)
