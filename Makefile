CC = gcc

SRC_DIR = src
TESTS_DIR = tests
BUILD_DIR = $(TESTS_DIR)/build

.PHONY: all clean

all: cpwn tests

cpwn:
	$(CC) $(SRC_DIR)/*.c $(TESTS_DIR)/cpwn.c -o $(BUILD_DIR)/cpwn

tests:
	$(CC) $(SRC_DIR)/*.c $(TESTS_DIR)/tests.c -o $(BUILD_DIR)/tests

clean:
	rm -rf $(BUILD_DIR)/*
