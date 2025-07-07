#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define chdir _chdir
#define getcwd _getcwd
#else
#include <unistd.h>
#include <dirent.h>
#endif

int hnd_mkdir(char **argv, size_t argc)
{
    if (argc < 2) {
        fprintf(stderr, "mkdir: missing operand\n");
        return 1;
    }
    #ifdef _WIN32
    if (_mkdir(argv[1]) != 0) 
    {
        perror("mkdir");
        return 1;
    }
    #else
        if (mkdir(argv[1], 0755) != 0) {
    
        perror("mkdir");
        return 1;
    }
    #endif
    return 0;
}
