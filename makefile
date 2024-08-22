GCC_FLAGS = -g -Wall -Wextra -Werror
CC = gcc

C_FILES = ./*.c
OUTPUT_FILE = main

main:
	$(CC) $(C_FILES) $(GCC_FLAGS) -o $(OUTPUT_FILE)

run: main
	./main

.PHONY: clean run main