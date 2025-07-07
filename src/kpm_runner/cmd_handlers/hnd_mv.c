#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int hnd_mv(char **argv, size_t argc)
{
    if (argc < 3) {
        fprintf(stderr, "mv: missing operand(s)\n");
        return 1;
    }
    if (rename(argv[1], argv[2]) != 0) {
        perror("mv");
        return 1;
    }
    return 0;
}
