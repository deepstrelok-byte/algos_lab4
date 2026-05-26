CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./src
SRC_DIR = src
BUILD_DIR = build
TARGET = lab4

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/rbtree.c
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/rbtree.o

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/rbtree.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rbtree.o: $(SRC_DIR)/rbtree.c $(SRC_DIR)/rbtree.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: all
	./$(TARGET)

test: all
	./$(TARGET) tests/test_input.txt test_output.txt
	cat test_output.txt

.PHONY: all clean run test