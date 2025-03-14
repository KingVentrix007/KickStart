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
#include "../templates/utils.h"
#include "stdbool.h"
#include <sys/wait.h>
int run_project()
{
    char *json_data;
    size_t file_size;
    FILE *fp = fopen("project.json","r");
    if(fp == NULL)
    {
        return -1;
    }
    fseek(fp,0L,SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    json_data = malloc(sizeof(char)*file_size+100);
    if(json_data == NULL)
    {
        fclose(fp);
        return -1;
    }
    fread(json_data,sizeof(char),file_size,fp);
    fclose(fp);

    json_t *root;
    json_error_t error;
    root = json_loads( json_data, 0, &error );
    json_t *run_cmd_json = json_object_get(root,"run_command");
    json_t *prod_name_json = json_object_get(root,"name");
    char *prod_name = prod_name_json && json_is_string(prod_name_json) ? strdup(json_string_value(prod_name_json)) : NULL;
    
    char *run_cmd = run_cmd_json && json_is_string(run_cmd_json) ? strdup(json_string_value(run_cmd_json)) : NULL;
    if(prod_name == NULL)
    {
        printf("Error getting project name: Run code like this: %s\n",run_cmd);
    }
    char *run_cmd_form = replace_string(run_cmd,"${project_name}",prod_name);
    // printf("Run cmd: DEBUG: %s\n",run_cmd_form);
    if(run_cmd_form == NULL)
    {
        printf("No valid run command found\n");
        return -1;
    }
    // size_t cmd_len = strlen(run_cmd_form);
    bool multi_cmds = false;
    // for (size_t i = 0; i < cmd_len; i++)
    // {
    //     printf("%c\n",run_cmd_form[i]);
    //     if(run_cmd_form[i] == '#')
    //     {
            
    //         multi_cmds = true;
    //     }
    // }
    if(multi_cmds == false)
    {
        system(run_cmd_form);

    }
    else
    {
        char *delimiter = "#";
        char *token = strtok(run_cmd_form, delimiter);

        while (token != NULL) {
            printf("Executing: %s\n", token);
            int status = system(token) != -1;  // Runs the command
            if (status == -1) {
                perror("system() failed");
                return 1;
            }
        
            if (WIFEXITED(status)) {  // Check if command exited normally
                int exit_code = WEXITSTATUS(status);
                if (exit_code == 0) {
                    printf("Command executed successfully!\n");
                } else {
                    return -1;
                }
            } else {
                return -2;
            }
        
            token = strtok(NULL, delimiter);
        }
    }
    // json_t *build_cmd;

   return 0;
}
int build_project()
{
    char *json_data;
    size_t file_size;
    FILE *fp = fopen("project.json","r");
    if(fp == NULL)
    {
        return -1;
    }
    fseek(fp,0L,SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    json_data = malloc(sizeof(char)*file_size+100);
    if(json_data == NULL)
    {
        fclose(fp);
        return -1;
    }
    fread(json_data,sizeof(char),file_size,fp);
    fclose(fp);

    json_t *root;
    json_error_t error;
    root = json_loads( json_data, 0, &error );
    json_t *build_cmd_json = json_object_get(root,"build_command");
    json_t *prod_name_json = json_object_get(root,"name");
    char *prod_name = prod_name_json && json_is_string(prod_name_json) ? strdup(json_string_value(prod_name_json)) : NULL;
    char *build_cmd = build_cmd_json && json_is_string(build_cmd_json) ? strdup(json_string_value(build_cmd_json)) : NULL;
    char *build_cmd_form = replace_string(build_cmd,"${project_name}",prod_name);
    if(build_cmd == NULL)
    {
        printf("No valid build command found\n");
        return -1;
    }
    // size_t cmd_len = strlen(build_cmd_form);
    bool multi_cmds = false;
    // for (size_t i = 0; i < cmd_len; i++)
    // {
    //     printf("%c\n",build_cmd_form[i]);
    //     if(build_cmd_form[i] == '#')
    //     {
            
    //         multi_cmds = true;
    //     }
    // }
    if(multi_cmds == false)
    {
        system(build_cmd_form);

    }
    else
    {
        char *delimiter = "#";
        char *token = strtok(build_cmd_form, delimiter);

        while (token != NULL) {
            printf("Executing: %s\n", token);
            int status = system(token) != -1;  // Runs the command
            if (status == -1) {
                perror("system() failed");
                return 1;
            }
        
            if (WIFEXITED(status)) {  // Check if command exited normally
                int exit_code = WEXITSTATUS(status);
                if (exit_code == 0) {
                    printf("Command executed successfully!\n");
                } else {
                    printf("Command failed with exit code: %d\n", exit_code);
                    return -1;
                }
            } else {
                printf("Command did not exit normally.\n");
                return -2;
            }
        
            token = strtok(NULL, delimiter);
        }
    }
    // system(build_cmd);
    // json_t *build_cmd;

   return 0;
}