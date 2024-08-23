GCC_FLAGS = -g -Wall -Wextra -Werror
CC = gcc

C_FILES = ./src/*.c
OUTPUT_FILE = main

main:
	$(CC) $(C_FILES) $(GCC_FLAGS) -o $(OUTPUT_FILE)

run: main
	./main
clean:
	rm -f $(OUTPUT_FILE)
.PHONY: clean run main