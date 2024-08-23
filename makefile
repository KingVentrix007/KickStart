CC = gcc
CFLAGS = -g -Wall -Wextra -Werror
LDFLAGS = -lcurl

# Automatically gather all .c files in the current directory and subdirectories, excluding the tests directory
SRCS = $(shell find . -path ./tests -prune -o -name '*.c' -print)
OBJS = $(SRCS:.c=.o)
TARGET = main

# Default rule
all: $(TARGET)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link the program
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

run:
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
