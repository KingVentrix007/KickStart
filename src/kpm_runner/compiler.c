#include <stdio.h>
#include "../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "compile_linux.h"
#include "compile_windows.h"


//Takes in users file
int compile(char **data,char *lang,size_t data_count)
{
    #if defined(_WIN32)
    // printf("HELLO\n");
    return compile_windows(data,lang,data_count);
    #elif defined(__APPLE__)
    // macOS specific compilation logic
    printf("Compiling on macOS with language: %s\n", lang);
    // This is a placeholder, actual implementation will depend on the language and compiler used
    return 0;
    #else
    // Linux or other Unix-like systems
    printf("Compiling on Linux with language: %s\n", lang);
    return compile_linux(data,lang,data_count);
    #endif
}