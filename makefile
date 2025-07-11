CC = gcc
CFLAGS = -g -Wall -Wextra -Werror
LDFLAGS = -lcurl -ljansson

# Directories
SRC_DIR = src
OBJ_DIR = obj
TARGET = kpm

# Find all subdirectories of SRC_DIR
INCLUDE_DIRS := $(shell find $(SRC_DIR) -type d)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))
CFLAGS += $(INCLUDE_FLAGS)

# Generate the list of source files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default rule (build with DEBUG defined)
all: $(TARGET)

# Build without DEBUG defined
build: CFLAGS := $(filter-out -DDEBUG,$(CFLAGS))
build: $(TARGET)
	mkdir -p "build"
	cp $(TARGET) "build"

# Create object directory structure
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the program
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

run: $(TARGET)
	mkdir -p "tests"
	(cd tests && ../$(TARGET) init)
run-d:
	mkdir -p "tests"
	(cd tests && valgrind --track-origins=yes --leak-check=full ../$(TARGET) init)
# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) build

.PHONY: all build run clean
