#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int hnd_echo(char **argv, size_t argc)
{
    for (size_t i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i + 1 < argc) printf(" ");
    }
    printf("\n");
    return 0;
}