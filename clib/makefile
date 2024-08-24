# Makefile

# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I/usr/include
LDFLAGS = -lcurl -ljansson
TARGET = libmanager
SRC = main.c

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

# Run the program with a package name
run: $(TARGET)
	@echo "Usage: make run PACKAGE_NAME=<name>"
	@echo "Example: make run PACKAGE_NAME=libfoo"
	@echo "Running with package name: $(PACKAGE_NAME)"
	./$(TARGET) libcurl

.PHONY: all clean run
