#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct makefile_config {
    char *project_name;
    char *project_language;
    char *project_dependencies;
    char *compiler;
    char *flags;
    char *output_file;
    char **source_dirs;
    size_t source_dir_count;
    char *makefile_path;
    char **file_extensions;
    size_t file_extension_count;
} makefile_config_t;

int create_makefile(makefile_config_t *config) {
    FILE *file = fopen("Makefile", "w");
    if (!file) {
        perror("Failed to create Makefile");
        return -1;
    }

    fprintf(file, "PROJECT_NAME := %s\n", config->project_name);
    fprintf(file, "LANGUAGE := %s\n", config->project_language);
    fprintf(file, "DEPENDENCIES := %s\n", config->project_dependencies);
    fprintf(file, "COMPILER := %s\n", config->compiler);
    fprintf(file, "\n# Compiler settings\n");
    fprintf(file, "CFLAGS := %s\n", config->flags);
    // Add rules for building the project
    printf("Finding source files in directories:\n");
    for (size_t i = 0; i < config->source_dir_count; i++)
    {
        
    }
    
    
    fprintf(file, "\nall:\n");
    fprintf(file, "\t@echo Building $(PROJECT_NAME) in $(LANGUAGE) with dependencies $(DEPENDENCIES)\n");

    fclose(file);
    return 0;
}