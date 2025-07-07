#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hnd_cp(char **argv, size_t argc)
{
    if (argc < 3) {
        fprintf(stderr, "cp: missing operand(s)\n");
        return 1;
    }
    // Basic copy of file (no recursive copy of directories)
    FILE *src = fopen(argv[1], "rb");
    if (!src) { perror("cp source"); return 1; }
    FILE *dst = fopen(argv[2], "wb");
    if (!dst) { perror("cp dest"); fclose(src); return 1; }

    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, n, dst) != n) {
            perror("cp write");
            fclose(src); fclose(dst);
            return 1;
        }
    }
    fclose(src);
    fclose(dst);
    return 0;
}