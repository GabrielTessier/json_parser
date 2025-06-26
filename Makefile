CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -g -O2 -Iinclude/ -fPIC

SRC_DIR=src
OBJ_DIR=obj
LIB_DIR=lib

SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

.PHONY: all

all: lib/libjson.so build-tests

$(LIB_DIR)/libjson.so: makedir $(OBJ_FILES)
	$(CC) -shared $(OBJ_FILES) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

build-tests:
	make -C tests/

.PHONY: clean makedir

makedir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIB_DIR)

clean:
	rm -rf obj lib
	make -C tests/ clean

