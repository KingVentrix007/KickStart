#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hnd_rm(char **argv, size_t argc)
{
    if (argc < 2) {
        fprintf(stderr, "rm: missing operand\n");
        return 1;
    }
    if (remove(argv[1]) != 0) {
        perror("rm");
        return 1;
    }
    return 0;
}