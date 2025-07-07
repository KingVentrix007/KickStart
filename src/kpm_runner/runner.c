#include <stdio.h>
#include "../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "compile_linux.h"
#include "compile_utils.h"
#include "compile_windows.h"
#include "../compile.h"
int hnd_cd(char **argv, size_t argc);

char **extract_cmd_lines(char *path,size_t *line_count)
{
    FILE *kpm_script_file = fopen(path,"r");
    if(kpm_script_file == NULL)
    {
        fprintf(stderr,"ERROR: Failed to open kpm_script_file at path %s\n",path);
        return NULL;
    }
    fseek(kpm_script_file, 0L, SEEK_END);
    size_t kpm_script_file_size = ftell(kpm_script_file);
    rewind(kpm_script_file);
    char *kpm_script_data = (char *)malloc(kpm_script_file_size*2+1);
    if(kpm_script_data == NULL)
    {
        fprintf(stderr,"ERROR: Failed to allocate memory for kpm_script_data");
        fclose(kpm_script_file);
        return NULL;
    }
    fread(kpm_script_data,sizeof(char),kpm_script_file_size,kpm_script_file);
    fclose(kpm_script_file);
    char **kpm_cmd_lines = (char **)malloc(kpm_script_file_size);
    size_t kpm_cmd_line_count = 0;
    if(kpm_cmd_lines == NULL)
    {
        fprintf(stderr,"ERROR: Failed to allocate memory for kpm_cmd_lines");
        free(kpm_script_data);
        return NULL;
    }
    //Extract lines
    char *line = strtok(kpm_script_data, "\n");
    kpm_cmd_lines[kpm_cmd_line_count] = strdup(line);
    kpm_cmd_line_count++;
    while (line != NULL) {
        printf("Line: %s\n", line);
        line = strtok(NULL, "\n");
        kpm_cmd_lines[kpm_cmd_line_count] = strdup(line);
        kpm_cmd_line_count++;
    }
    *line_count = kpm_cmd_line_count;
    return kpm_cmd_lines;
}

char **parse_command(char *command,size_t *argc)
{
    char *internal_cmd = strdup(command);
    char **command_parts = malloc(strlen(internal_cmd)+10);
    if(command_parts == NULL)
    {
        fprintf(stderr,"ERROR(parse_command): Failed to allocate memory\n");
        *argc = 0;
        return NULL;
    }
    char *line = strtok(internal_cmd, "\n");
    while (line != NULL) {
    printf("Line: %s\n", line);
    line = strtok(NULL, "\n");
    command_parts[*argc] = strdup(line);
    *argc++;
    }
    return command_parts;
}

int execute(char **commands,size_t command_count)
{
    char **current_command = NULL;
    size_t current_command_index = 0;
    int ret = 0;
    while (strcmp(current_command, "exit") != 0 && current_command_index < command_count)
    {
        size_t curr_cmd_size = 0;
        current_command = parse_command(commands[current_command_index], &curr_cmd_size);

        // Extract command name (first word)
        char command_name[64] = {0};
        sscanf(current_command, "%63s", command_name); // Safely extract the command keyword

        if (strcmp(command_name, "compile") == 0) {
            ret = cmd_compile(current_command,curr_cmd_size);
            
        }
        else if (strcmp(command_name, "run") == 0) {
            //
        }
        else if (strcmp(command_name, "cd") == 0) {
            ret = hnd_cd(current_command,curr_cmd_size);
        }
        else if (strcmp(command_name, "cp") == 0) {
            // Copy a file or folder
        }
        else if (strcmp(command_name, "mv") == 0) {
            // Move or rename a file or folder
        }
        else if (strcmp(command_name, "rm") == 0) {
            // Delete a file or folder
        }
        else if (strcmp(command_name, "mkdir") == 0) {
            // Create a directory
        }
        else if (strcmp(command_name, "echo") == 0) {
            // Print a message to the terminal
        }
        else if (strcmp(command_name, "exit") == 0) {
            // Exit the script with optional code
            break;
        }
        else if (strcmp(command_name, "find") == 0) {
            // Find a file matching an expression
        }
        else if (strcmp(command_name, "env") == 0) {
            // Set or get an environment variable
        }
        else if (strcmp(command_name, "if") == 0) {
            // Start a conditional block
        }
        else if (strcmp(command_name, "else") == 0) {
            // Start the "else" part of a conditional block
        }
        else if (strcmp(command_name, "endif") == 0) {
            // End a conditional block
        }
        else if (strcmp(command_name, "platform") == 0) {
            // Get the current OS platform
        }
        else if (strcmp(command_name, "shell") == 0) {
            // Run a raw shell command
        }
        else if (strcmp(command_name, "set") == 0) {
            // Define a variable
        }
        else if (strcmp(command_name, "print") == 0) {
            // Print the value of a variable
        }
        else if (strcmp(command_name, "exists") == 0) {
            // Check if a file or folder exists
        }
        else if (strcmp(command_name, "wait") == 0) {
            // Wait for a number of seconds or milliseconds
        }
        else {
            fprintf(stderr, "Unknown command: %s\n", command_name);
        }

        current_command_index++;
        if(ret != 0)
        {
            current_command_index--;
            printf("Failed execution at line %ld:(%s) with error %d\n",current_command_index,commands[current_command_index],ret);
            return ret;
        }
    }

}

int kpm_script_runner(char *path)
{
    size_t kpm_cmd_line_count = 0;
    char **kpm_cmd_lines = extract_cmd_lines(path,&kpm_cmd_line_count);
    if(kpm_cmd_lines == NULL)
    {
        fprintf(stderr,"ERROR(kpm_script_runner): Failed to extract lines for kpm_cmd_lines\n");
        return -1;
    }
    return execute(kpm_cmd_lines,kpm_cmd_line_count);
    return 0;

}