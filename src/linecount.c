#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include "jansson.h"
// #include "fetch.h"
#include <libgen.h>
#include <dirent.h>
#include "errno.h"
#include "templates/utils.h"
#include "stdbool.h"
#include <sys/wait.h>
#include <dirent.h> 
#include <ctype.h>
void listFilesRecursively(const char *basePath);
char** get_extensions(size_t *count)
{
    char *json_data;
    size_t file_size;
    FILE *fp = fopen("project.json","r");
    if(fp == NULL)
    {
        return NULL;
    }
    fseek(fp,0L,SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    json_data = malloc(sizeof(char)*file_size+100);
    if(json_data == NULL)
    {
        fclose(fp);
        return NULL;
    }
    fread(json_data,sizeof(char),file_size,fp);
    fclose(fp);

    json_t *root;
    json_error_t error;
    root = json_loads( json_data, 0, &error );
    json_t *program_extensions = json_object_get(root,"extensions");
    if (!json_is_array(program_extensions)) {
        printf("Error: 'extensions' is not an array\n");
        json_decref(root);
        return NULL;
    }
    size_t extensions_count = json_array_size(program_extensions);
    printf("Extensions count: %zu\n", extensions_count);

    // Allocate memory for C array of strings
    char **extensions = malloc(extensions_count * sizeof(char *));
    if (!extensions) {
        printf("Memory allocation failed\n");
        json_decref(root);
        return NULL;
    }

    // Read JSON array into C array
    for (size_t i = 0; i < extensions_count; i++) {
        json_t *value = json_array_get(program_extensions, i);
        if (json_is_string(value)) {
            const char *ext = json_string_value(value);
            extensions[i] = strdup(ext);  // Copy string to new memory
        } else {
            extensions[i] = NULL;  // Handle non-string values safely
        }
    }
    *count = extensions_count;
    // Print extensions
    return extensions;
}

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot ;
}
int is_blank_or_empty(const char *line) {
    // Check if the line is empty
    if (line == NULL || *line == '\0') {
        return 1; // Empty string
    }

    // Check if the line only contains whitespace
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return 0;  // Found a non-whitespace character
        }
        line++;
    }

    return 1; // Only whitespace characters
}


int count_lines(const char *path)
{
    size_t num_lines = 0;
    FILE *file_to_read = fopen(path,"r");
    if(file_to_read == NULL)
    {
        return -1;
    }
    fseek(file_to_read,0l,SEEK_END);
    size_t file_size = ftell(file_to_read);
    rewind(file_to_read);
    char *data = malloc(file_size+50);
    if(data == NULL)
    {
        return -1;
    }
    fread(data,sizeof(char),file_size,file_to_read);
    char *line = data;
    while (line != NULL && *line != '\0') {
        char *newline_pos = strchr(line, '\n');  // Find the end of the line
        if (newline_pos != NULL) {
            *newline_pos = '\0';  // Null-terminate the line
            printf("Line: %s\n", line);
            if(is_blank_or_empty(line) == 0)
            {
                num_lines++;
            }
            line = newline_pos + 1;  // Move to the next line
        } else {
            // Last line without a newline character
            printf("Line: %s\n", line);
            if(is_blank_or_empty(line) == 0)
            {
                num_lines+=1;
            }
            break;
        }
    }

    free(data);
    return num_lines;
}
void listFilesRecursively(const char *basePath) {
    size_t extensions_count;
    char **extensions = get_extensions(&extensions_count);
    struct dirent *de;
    DIR *dr = opendir(basePath);

    if (dr == NULL) {
        printf("Could not open directory: %s\n", basePath);
        return;
    }
    FILE *file = fopen("kickstart_analytics.txt", "a");  // Open the file in append mode
    if (file == NULL) {
        printf("Could not open kickstart_analytics.txt\n");
        closedir(dr);
        return;
    }

    while ((de = readdir(dr)) != NULL) {
        char path[1024];
        struct stat statbuf;

        // Ignore "." and ".." to prevent infinite recursion
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0 || de->d_name[0] == '.')
            continue;

        snprintf(path, sizeof(path), "%s/%s", basePath, de->d_name);

        // Get file status
        if (stat(path, &statbuf) == 0) {
            printf("%s\n", path);
            const char *filetype = get_filename_ext(path);
            printf("filetype == [%s]\n",filetype);
            size_t num_lines = -1;
            // printf("Hello\n");
            for (size_t i = 0; i < extensions_count; i++)
            {
                // printf(">>\n");
                // printf(">>%s\n",extensions[i]);
                if(strcmp(extensions[i],filetype) == 0)
                {
                    // printf("Valid\n");
                    num_lines =  count_lines(path);
                }
            }
            size_t val = -1;
            // printf("Check\n");
            if (num_lines != val) {
                printf("Write\n");
                fprintf(file, "File: %s, Number of lines: %zu\n", path, num_lines);
            }
            // If it's a directory, recursively call the function
            if (S_ISDIR(statbuf.st_mode)) {
                listFilesRecursively(path);
            }
        }
    }

    closedir(dr);
}
int get_line_count()
{
    listFilesRecursively(".");
    return 0;
}