CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS = -lcurl -ljansson

# Directories
SRC_DIR = src
OBJ_DIR = obj
TARGET = kpm

# Generate the list of source files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default rule
all: $(TARGET)

# Create object directory structure
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the program
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

run: $(TARGET)
	./$(TARGET) init

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all run clean
