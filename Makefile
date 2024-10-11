
CC=gcc
OUT_DIR=./bin
OUT_FILE=$(OUT_DIR)/blade
TEST_FILE=./src/test.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Wextra -std=c11 -ggdb
SRC=./src/main.c
LIBS=-lncursesw -lm -pthread
# dir
LOCAL_BIN=/usr/local/bin/blade
STATICD = ./static
OBJD    = ./obj
DYND    = ./dynamic
AUDIOLIB=$(OUT_DIR)/libaudiolib.so
BLADELIB=$(OUT_DIR)/libBLADE.so
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
	@# @$CC) $(CFLAGS) -shared -o $(AUDIOLIB) audioplayer.o audio.o

bladeapi:
	@echo ">> compiling bladeapi"
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(COMMONAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(EDITORAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(VISUALAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(TOKENIZERAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(FILEBROWSERAPISRC) $(LIBS) -fPIC
	@$(CC) $(CFLAGS) $(INCLUDE)  -c $(CFGSRC) $(LIBS) -fPIC
	@mv *.o $(OBJD)
	@# uncomment if u need to also compile dyn libs.
	@#$(CC) $(CFLAGS) -shared -o $(BLADELIB) $(OBJD)/*.o
	
merge:
	@echo ">> merging lib internal/external libs"
	@ar rcs $(FINAL_LIB) $(OBJD)/*.o
	@mv $(FINAL_LIB) $(STATICD)
deps: audioapi bladeapi merge

main: merge
	@echo ">> compiling the final executable"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $(OUT_FILE) $(SRC) $(STATICD)/$(FINAL_LIB) $(LIBS)
	@echo "To run the editor just run ./bin/blade :3"

run:
	@$(OUT_FILE) . ; reset

debug:
	@valgrind $(OUT_FILE) $(TEST_FILE)

install:
	sudo mv $(OUT_FILE) $(LOCAL_BIN)
	echo "blade was installed successfully (Into </usr/local/bin>), Run the \`blade\` Command to Start using the editor."
clean:
	@rm -rf $(OBJD)
	@rm -rf *.o

fclean: clean
	@rm -rf $(STATICD)
	@rm -rf $(DYND)
	@rm -rf $(OUT_DIR)

re: fclean all

# Remake blade api then merge.
reb: bladeapi bin
rea: audioapi bin

