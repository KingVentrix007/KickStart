#include <stdio.h>
#include "../../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "../compile_linux.h"
#include "compile_windows.h"
#include "../../compile.h"


int cmd_compile(char **argv,size_t argc)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s compile <file1> [file2...]\n", argv[0]);
        return 1;
    }
    char **files = malloc((argc - 2) * sizeof(char *));
    if (!files) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }
    char lang[1024] = "cpp"; // Default language
    //Extract language from file extension if provided
    char *lang_ext = strrchr(argv[2], '.');
    if (lang_ext){
        strcpy(lang, lang_ext + 1); // Copy the extension without the dot
    }
    //Copy file names into the array
    for (int i = 2; i < argc; ++i) {
        files[i - 2] = strdup(argv[i]);
        if (!files[i - 2]) {
            fprintf(stderr, "Memory allocation failed for file name.\n");
            for (int j = 0; j < i - 2; ++j) {
                free(files[j]);
            }
            free(files);
            return 1;
        }
    }
    int files_count = argc - 2;
    return compile(files, lang,files_count);
}