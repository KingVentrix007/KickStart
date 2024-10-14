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
    char *run_cmd = run_cmd_json && json_is_string(run_cmd_json) ? strdup(json_string_value(run_cmd_json)) : NULL;
    if(run_cmd == NULL)
    {
        printf("No valid run command found\n");
        return -1;
    }
    system(run_cmd);
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
    char *build_cmd = build_cmd_json && json_is_string(build_cmd_json) ? strdup(json_string_value(build_cmd_json)) : NULL;
    if(build_cmd == NULL)
    {
        printf("No valid build command found\n");
        return -1;
    }
    system(build_cmd);
    // json_t *build_cmd;

   return 0;
}