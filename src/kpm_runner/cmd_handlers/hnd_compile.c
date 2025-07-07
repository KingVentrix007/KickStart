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
    // if (argc < 3) {
    //     fprintf(stderr, "Usage: %s compile <file1> [file2...]\n", argv[0]);
    //     return 1;
    // }
    char **files = malloc((100) * sizeof(char *));
    if (!files) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }
    char lang[1024] = "cpp"; // Default language
    //Extract language from file extension if provided
    char *lang_ext = strrchr(argv[1], '.');
    if (lang_ext){
        strcpy(lang, lang_ext + 1); // Copy the extension without the dot
    }
    //Copy file names into the array
    for (int i = 1; i < argc; ++i) {
        files[i-1] = strdup(argv[i]);
        printf("File: %s\n",files[i-1]);
        if (!files[i]) {
            fprintf(stderr, "Memory allocation failed for file name.\n");
            for (int j = 0; j < i - 1; ++j) {
                free(files[j]);
            }
            free(files);
            return 1;
        }
    }
    int files_count = argc-1;
    printf("Files count %d:%s\n",files_count,files[1]);
    
    return compile(files, lang,files_count);
}