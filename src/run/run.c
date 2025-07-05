#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>
#include "../templates/utils.h"  // For replace_string()

#ifdef _WIN32
  #include <windows.h>
  #include <process.h>  // For _getpid if needed
  #define IS_WINDOWS 1
#else
  #include <sys/wait.h>
  #define IS_WINDOWS 0
#endif

#if IS_WINDOWS
  // On Windows, WIFEXITED and WEXITSTATUS don't exist, define equivalents
  #define WIFEXITED(status) (((status) & 0xFF) == 0)
  #define WEXITSTATUS(status) (((status) >> 8) & 0xFF)
#endif

// Portable wrapper for system() to check command success/failure
int run_system_cmd(const char *cmd)
{
    int status = system(cmd);
    if (status == -1) {
        perror("system() failed");
        return -1;
    }

    #if IS_WINDOWS
        // system() returns the actual exit code directly on Windows
        int exit_code = status;
        if (exit_code == 0) {
            printf("Command executed successfully!\n");
            return 0;
        } else {
            printf("Command failed with exit code: %d\n", exit_code);
            return exit_code;
        }
    #else
        // On POSIX systems, analyze status with macros
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                printf("Command executed successfully!\n");
                return 0;
            } else {
                printf("Command failed with exit code: %d\n", exit_code);
                return exit_code;
            }
        } else {
            printf("Command did not exit normally.\n");
            return -2;
        }
    #endif
}

int run_project()
{
    char *json_data = NULL;
    size_t file_size;
    FILE *fp = fopen("project.json","r");
    if(fp == NULL)
    {
        fprintf(stderr, "Failed to open project.json\n");
        return -1;
    }
    fseek(fp,0L,SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    json_data = malloc(file_size + 1);  // +1 for null terminator
    if(json_data == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    fread(json_data, 1, file_size, fp);
    json_data[file_size] = '\0';  // null terminate
    fclose(fp);

    json_error_t error;
    json_t *root = json_loads(json_data, 0, &error);
    free(json_data);
    if(!root) {
        fprintf(stderr, "JSON parsing error on line %d: %s\n", error.line, error.text);
        return -1;
    }

    json_t *run_cmd_json = json_object_get(root,"run_command");
    json_t *prod_name_json = json_object_get(root,"name");
    char *prod_name = prod_name_json && json_is_string(prod_name_json) ? strdup(json_string_value(prod_name_json)) : NULL;
    
    char *run_cmd = run_cmd_json && json_is_string(run_cmd_json) ? strdup(json_string_value(run_cmd_json)) : NULL;

    if(prod_name == NULL) {
        fprintf(stderr, "Error: Project name not found in JSON.\n");
        if(run_cmd) printf("Run code like this: %s\n", run_cmd);
    }

    if(run_cmd == NULL) {
        fprintf(stderr, "No valid run_command found in JSON.\n");
        if(prod_name) free(prod_name);
        json_decref(root);
        return -1;
    }

    char *run_cmd_form = replace_string(run_cmd, "${project_name}", prod_name ? prod_name : "");
    free(run_cmd);
    if(prod_name) free(prod_name);

    if(run_cmd_form == NULL) {
        fprintf(stderr, "Failed to prepare run command string.\n");
        json_decref(root);
        return -1;
    }

    // Check for multiple commands separated by '#'
    if(strchr(run_cmd_form, '#') == NULL) {
        int status = run_system_cmd(run_cmd_form);
        free(run_cmd_form);
        json_decref(root);
        return status;
    } else {
        char *delimiter = "#";
        char *to_free = run_cmd_form;  // strtok modifies the string
        char *token = strtok(run_cmd_form, delimiter);

        while(token != NULL) {
            printf("Executing: %s\n", token);
            int status = run_system_cmd(token);
            if(status != 0) {
                free(to_free);
                json_decref(root);
                return status;
            }
            token = strtok(NULL, delimiter);
        }
        free(to_free);
        json_decref(root);
        return 0;
    }
}

int build_project()
{
    char *json_data = NULL;
    size_t file_size;
    FILE *fp = fopen("project.json","r");
    if(fp == NULL)
    {
        fprintf(stderr, "Failed to open project.json\n");
        return -1;
    }
    fseek(fp,0L,SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    json_data = malloc(file_size + 1);  // +1 for null terminator
    if(json_data == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    fread(json_data, 1, file_size, fp);
    json_data[file_size] = '\0';  // null terminate
    fclose(fp);

    json_error_t error;
    json_t *root = json_loads(json_data, 0, &error);
    free(json_data);
    if(!root) {
        fprintf(stderr, "JSON parsing error on line %d: %s\n", error.line, error.text);
        return -1;
    }

    json_t *build_cmd_json = json_object_get(root,"build_command");
    json_t *prod_name_json = json_object_get(root,"name");
    char *prod_name = prod_name_json && json_is_string(prod_name_json) ? strdup(json_string_value(prod_name_json)) : NULL;
    
    char *build_cmd = build_cmd_json && json_is_string(build_cmd_json) ? strdup(json_string_value(build_cmd_json)) : NULL;

    if(build_cmd == NULL) {
        fprintf(stderr, "No valid build_command found in JSON.\n");
        if(prod_name) free(prod_name);
        json_decref(root);
        return -1;
    }

    char *build_cmd_form = replace_string(build_cmd, "${project_name}", prod_name ? prod_name : "");
    free(build_cmd);
    if(prod_name) free(prod_name);

    if(build_cmd_form == NULL) {
        fprintf(stderr, "Failed to prepare build command string.\n");
        json_decref(root);
        return -1;
    }

    // Check for multiple commands separated by '#'
    if(strchr(build_cmd_form, '#') == NULL) {
        int status = run_system_cmd(build_cmd_form);
        free(build_cmd_form);
        json_decref(root);
        return status;
    } else {
        char *delimiter = "#";
        char *to_free = build_cmd_form;  // strtok modifies the string
        char *token = strtok(build_cmd_form, delimiter);

        while(token != NULL) {
            printf("Executing: %s\n", token);
            int status = run_system_cmd(token);
            if(status != 0) {
                free(to_free);
                json_decref(root);
                return status;
            }
            token = strtok(NULL, delimiter);
        }
        free(to_free);
        json_decref(root);
        return 0;
    }
}
