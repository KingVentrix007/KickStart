#include <stdio.h>
#include "../../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "../compile_linux.h"
#include "compile_windows.h"
#include "../../compile.h"


extern char *get_variable_value(const char *name);
extern char *trim_whitespace(char *str);


// Helper to split a space-separated string into tokens
char **split_whitespace(const char *input, size_t *count) {
    char *temp = strdup(input);
    if (!temp) return NULL;

    size_t capacity = 10;
    char **result = malloc(capacity * sizeof(char *));
    *count = 0;

    char *token = strtok(temp, " \t\r\n");
    while (token) {
        if (*count >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(char *));
        }
        result[*count] = strdup(token);
        (*count)++;
        token = strtok(NULL, " \t\r\n");
    }

    free(temp);
    return result;
}

int cmd_compile(char **argv, size_t argc) {
    size_t max_files = 100;
    char **files = malloc(max_files * sizeof(char *));
    if (files == NULL) {
        // free(files);
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    size_t file_index = 0;
    char lang[1024] = "cpp"; // Default language

    for (size_t i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        char *resolved_arg = NULL;

        if (arg[0] == '$') {
            char *value = get_variable_value(arg + 1);
            if (!value) {
                fprintf(stderr, "Undefined variable: %s\n", arg + 1);
                for (size_t j = 0; j < file_index; ++j) free(files[j]);
                free(files);
                return 1;
            }

            size_t split_count = 0;
            char **split_files = split_whitespace(value, &split_count);
            if (split_files == NULL) {
                fprintf(stderr, "Memory split failed.\n");
                for (size_t k = 0; k < file_index; ++k) free(files[k]);
                free(files);
                
                return 1;
            }

            for (size_t j = 0; j < split_count; ++j) {
                if (file_index >= max_files) {
                    max_files *= 2;
                    files = realloc(files, max_files * sizeof(char *));
                }

                files[file_index] = strdup(split_files[j]);
                if (!files[file_index]) {
                    fprintf(stderr, "Memory allocation failed.\n");
                    for (size_t k = 0; k < file_index; ++k) free(files[k]);
                    free(files);
                    for (size_t l = j; l < split_count; l++)
                    {
                        free(split_files[l]);
                    }
                    free(split_files);
                    
                    return 1;
                }

                // Set language from first real file
                if (file_index == 0) {
                    char *ext = strrchr(split_files[j], '.');
                    if (ext) strcpy(lang, ext + 1);
                }

                file_index++;
                free(split_files[j]);
            }
            free(split_files);
        } else {
            if (file_index >= max_files) {
                max_files *= 2;
                files = realloc(files, max_files * sizeof(char *));
            }

            resolved_arg = strdup(arg);
            if (!resolved_arg) {
                fprintf(stderr, "Memory allocation failed.\n");
                for (size_t j = 0; j < file_index; ++j) free(files[j]);
                free(files);
                return 1;
            }

            files[file_index] = resolved_arg;

            if (file_index == 0) {
                char *ext = strrchr(resolved_arg, '.');
                if (ext) strcpy(lang, ext + 1);
            }

            file_index++;
        }
    }

    // for (size_t i = 0; i < file_index; ++i) {
        // printf("File: %zu:%s\n", i, files[i]);
    // }

    int result = compile(files, lang, file_index);

    for (size_t i = 0; i < file_index; ++i) {
        free(files[i]);
    }
    free(files);

    return result;
}
