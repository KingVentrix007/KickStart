#include <stdio.h>
#include "../../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "../compile_linux.h"
#include "compile_windows.h"
#include "../../compile.h"

char *get_variable_value(const char *name);


int cmd_compile(char **argv, size_t argc)
{
    char **files = malloc(100 * sizeof(char *));
    if (!files) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    char lang[1024] = "cpp"; // Default language

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        char *resolved_arg;

        if (arg[0] == '$') {
            resolved_arg = get_variable_value(arg + 1);
            if (!resolved_arg) {
                fprintf(stderr, "Undefined variable: %s\n", arg + 1);
                // free previously allocated memory
                for (int j = 0; j < i - 1; ++j) free(files[j]);
                free(files);
                return 1;
            }
        } else {
            resolved_arg = arg;
        }

        files[i - 1] = strdup(resolved_arg);
        if (!files[i - 1]) {
            fprintf(stderr, "Memory allocation failed for file name.\n");
            for (int j = 0; j < i - 1; ++j) free(files[j]);
            free(files);
            return 1;
        }

        // Extract language from first valid file if applicable
        if (i == 1) {
            char *lang_ext = strrchr(resolved_arg, '.');
            if (lang_ext) {
                strcpy(lang, lang_ext + 1); // Copy the extension without the dot
            }
        }
    }

    int files_count = argc - 1;
    for (size_t i = 0; i < files_count; i++)
    {
        printf("File: %d:%s\n",i,files[i]); 
    }
    
    return compile(files, lang, files_count);
}
