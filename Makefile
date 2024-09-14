CC=gcc
OUT_DIR=./bin
OUT_FILE=$(OUT_DIR)/mi
TEST_FILE=./src/test.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Wextra -std=c11
SRC=./src/main.c
LIBS=-lncursesw -lm -pthread
AUDIOLIB=$(OUT_DIR)/libaudiolib.so
MILIB=$(OUT_DIR)/libmi.so
FINAL_LIB=mlib.a


all: deps main
bin: main

audioapi:
	@echo ">> compiling miniaudio"
	@mkdir -p $(OUT_DIR)
	@$(CC) $(CFLAGS) $(INCLUDE) ./src/audio*.c -c $(LIBS) -fPIC
	@# uncomment if u need to also compile dyn libs.
	@# $(CC) $(CFLAGS) -shared -o $(AUDIOLIB) audioplayer.o audio.o

miapi:
	@echo ">> compiling miapi"
	@mkdir -p $(OUT_DIR)
	@$(CC) $(CFLAGS) $(INCLUDE) ./src/mi*.c -c $(LIBS) -fPIC
	@# uncomment if u need to also compile dyn libs.
	@#$(CC) $(CFLAGS) -shared -o $(MILIB) mi*.o

merge:
	@echo ">> merging lib internal/external libs"
	@ar rcs $(FINAL_LIB) *.o

deps: audioapi miapi merge

main: merge
	@mkdir -p $(OUT_DIR)
	@echo ">> compiling the final executable"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $(OUT_FILE)  $(SRC) $(FINAL_LIB) $(LIBS)
	@echo "To run the editor just run ./bin/mi :3"

run:
	@$(OUT_FILE) .

debug:
	@valgrind $(OUT_FILE) $(TEST_FILE)

clean:
	rm ./*.o


fclean: clean
	rm $(FINAL_LIB)
	rm $(OUT_FILE)
