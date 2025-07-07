#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>  // _chdir
#define chdir _chdir
#else
#include <unistd.h>
#endif

int hnd_cd(char **argv, size_t argc)
{
    if (argc < 2)
    {
        fprintf(stderr, "cd: missing operand\n");
        return 1;
    }

    const char *path = argv[1];

    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }

    // Optional: print new working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Now in directory: %s\n", cwd);
    }
    else
    {
        perror("getcwd");
    }

    return 0;
}
