CC = gcc
CFLAGS = -Wall -I include
SRC_DIR = src/
BIN_DIR = bin/
EXECUTABLE = $(BIN_DIR)tp3.out
SOURCE_FILES = $(SRC_DIR)main.c $(SRC_DIR)quadTree.c $(SRC_DIR)heap.c $(SRC_DIR)ponto.c $(SRC_DIR)hash.c  

all: 
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCE_FILES) -lm

read:
	 ./bin/tp3.out

clean:
	rm -rf $(BIN_DIR)*.out