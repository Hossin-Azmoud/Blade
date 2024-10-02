
CC=gcc
OUT_DIR=./bin
OUT_FILE=$(OUT_DIR)/mi
TEST_FILE=./src/test.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Wextra -std=c11 -ggdb
SRC=./src/main.c
LIBS=-lncursesw -lm -pthread
# dir
STATICD = ./static
OBJD    = ./obj
DYND    = ./dynamic
AUDIOLIB=$(OUT_DIR)/libaudiolib.so
MILIB=$(OUT_DIR)/libmi.so
FINAL_LIB=mlib.a

# Sources.
AUDIOAPISRC=./src/audio/*.c
COMMONAPISRC=./src/common/*.c
EDITORAPISRC=./src/editor/*.c
VISUALAPISRC=./src/visuals/*.c
TOKENIZERAPISRC=./src/tokenizer/*.c
FILEBROWSERAPISRC=./src/filebrowser/*.c
CFGSRC=./src/cfg/*.c

all: prepare_dir deps main
bin: main

prepare_dir:
	mkdir -p $(STATICD)
	mkdir -p $(OBJD)
	mkdir -p $(DYND)
	mkdir -p $(OUT_DIR)

audioapi:
	@echo ">> compiling miniaudio"
	@$(CC) $(CFLAGS) $(INCLUDE) $(AUDIOAPISRC) -c $(LIBS) -fPIC
	@mv *.o $(OBJD)
	@# uncomment if u need to also compile dyn libs.
	@$CC) $(CFLAGS) -shared -o $(AUDIOLIB) audioplayer.o audio.o

miapi:
	@echo ">> compiling miapi"
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(COMMONAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(EDITORAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(VISUALAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(TOKENIZERAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(FILEBROWSERAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(CFGSRC) $(LIBS) -fPIC
	@mv *.o $(OBJD)
	@# uncomment if u need to also compile dyn libs.
	@#$(CC) $(CFLAGS) -shared -o $(MILIB) mi*.o
	
merge:
	@echo ">> merging lib internal/external libs"
	@ar rcs $(FINAL_LIB) $(OBJD)/*.o
	@mv $(FINAL_LIB) $(STATICD)
deps: audioapi miapi merge

main: merge
	@echo ">> compiling the final executable"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $(OUT_FILE) $(SRC) $(STATICD)/$(FINAL_LIB) $(LIBS)
	@echo "To run the editor just run ./bin/mi :3"

run:
	@$(OUT_FILE) . ; reset

debug:
	@valgrind $(OUT_FILE) $(TEST_FILE)

clean:
	@rm -f $(OBJD)

fclean: clean
	@rm -f $(STATICD)
	@rm -f $(DYND)
	@rm -f $(OUT_DIR)
