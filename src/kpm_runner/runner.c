#include <stdio.h>
#include "../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "compile_linux.h"
#include "compile_utils.h"
#include "compile_windows.h"
#include "../compile.h"
#include "stdbool.h"
int hnd_cd(char **argv, size_t argc);
int hnd_cp(char **argv, size_t argc);
int hnd_mv(char **argv, size_t argc);
int hnd_rm(char **argv, size_t argc);
int hnd_mkdir(char **argv, size_t argc);
int hnd_echo(char **argv, size_t argc);
int hnd_env(char **argv, size_t argc);
int hnd_wait(char **argv, size_t argc);
char **hnd_find(char **argv,size_t argc);
int hnd_run();
int hnd_build();
char **extract_cmd_lines(char *path, size_t *line_count)
{
    FILE *kpm_script_file = fopen(path, "r");
    if (kpm_script_file == NULL) {
        fprintf(stderr, "ERROR: Failed to open kpm_script_file at path %s\n", path);
        return NULL;
    }

    fseek(kpm_script_file, 0L, SEEK_END);
    size_t kpm_script_file_size = ftell(kpm_script_file);
    rewind(kpm_script_file);

    char *kpm_script_data = (char *)calloc(kpm_script_file_size + 1, sizeof(char));
    if (kpm_script_data == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for kpm_script_data\n");
        fclose(kpm_script_file);
        return NULL;
    }

    fread(kpm_script_data, sizeof(char), kpm_script_file_size, kpm_script_file);
    fclose(kpm_script_file);

    size_t capacity = 64;
    char **kpm_cmd_lines = (char **)malloc(capacity * sizeof(char *));
    if (kpm_cmd_lines == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for kpm_cmd_lines\n");
        free(kpm_script_data);
        return NULL;
    }

    size_t kpm_cmd_line_count = 0;
    char *line = strtok(kpm_script_data, "\n");

    while (line != NULL) {
        if (kpm_cmd_line_count >= capacity) {
            capacity *= 2;
            char **tmp = realloc(kpm_cmd_lines, capacity * sizeof(char *));
            if (!tmp) {
                fprintf(stderr, "ERROR: realloc failed\n");
                break;
            }
            kpm_cmd_lines = tmp;
        }

        kpm_cmd_lines[kpm_cmd_line_count++] = strdup(line);  // LINE 68 (safe now)
        line = strtok(NULL, "\n");
    }

    kpm_cmd_lines[kpm_cmd_line_count] = NULL; // NULL-terminate the array
    *line_count = kpm_cmd_line_count;

    free(kpm_script_data);  // ✅ Free the data buffer now that lines are duplicated
    return kpm_cmd_lines;
}


char **parse_command(char *command, size_t *argc)
{
    char *internal_cmd = strdup(command);
    if (!internal_cmd) {
        fprintf(stderr, "ERROR(parse_command): Failed to duplicate command\n");
        *argc = 0;
        return NULL;
    }

    char **command_parts = malloc((strlen(internal_cmd) + 10) * sizeof(char *));
    if (!command_parts) {
        fprintf(stderr, "ERROR(parse_command): Failed to allocate memory\n");
        free(internal_cmd);
        *argc = 0;
        return NULL;
    }

    size_t count = 0;
    char *line = strtok(internal_cmd, " ");
    while (line != NULL) {
        command_parts[count] = strdup(line);
        // free(line);
        if (!command_parts[count]) {
            fprintf(stderr, "ERROR(parse_command): strdup failed\n");
            // Cleanup before exit
            for (size_t i = 0; i < count; ++i)
                free(command_parts[i]);
            free(command_parts);
            free(internal_cmd);
            *argc = 0;
            return NULL;
        }
        count++;
        line = strtok(NULL, " ");
    }

    *argc = count;
    free(internal_cmd);
    return command_parts;
}

typedef struct variable
{
    char name[100];
    char *value;
}variable_t;
variable_t all_vars[1024] = {0};
size_t num_current_vars = 0;

char *get_variable_value(const char *name) {
    for (size_t i = 0; i < num_current_vars; i++) {
        // printf("Var: %ld:%s\n",i,all_vars[i].name);
        if (strcmp(all_vars[i].name, name) == 0) {
            return all_vars[i].value;
        }
    }
    return NULL;  // Not found
}


int execute(char **commands,size_t command_count_in)
{
    char **current_command = NULL;
    char **current_command_save = NULL;
    size_t command_count = command_count_in;
     size_t curr_cmd_size = 0;
    size_t current_command_index = 0;
    // printf("First cmd %s\n",commands[current_command_index]);
    current_command = parse_command(commands[current_command_index], &curr_cmd_size);
    size_t og_cmd_size = curr_cmd_size;
    current_command_save = current_command;
    // printf("Current cmd 1 %s\n",current_command[0]);
    int ret = 0;
    bool var_set = false;
    char *var_name ;
    while (strcmp(current_command[0], "exit") != 0 && current_command_index < command_count)
    {
       
        

        // Extract command name (first word)
        char command_name[64] = {0};
        // printf("Current command: %s\n",current_command[0]);
        sscanf(current_command[0], "%63s", command_name); // Safely extract the command keyword
        // printf("Command name: %s\n",command_name);
        
        if (command_name[0] == '$')
        {
            // size_t name_len = strlen(command_name);
            // Case: $var=value
            char *equal_sign = strchr(command_name, '=');
            if (equal_sign != NULL)
            {
                var_set = true;

                // Split var name and value
                *equal_sign = '\0'; // null-terminate var name
                var_name = command_name + 1;
                // printf("Var name $: %s\n",var_name);
                // Store or set the variable here
                // set_variable(var_name, var_value);

                // Skip this command by moving current_command pointer forward
                current_command++; // Skip the $var=value part
                curr_cmd_size--;   // Adjust count too
            }
            // Case: $var = value
            else if (strcmp(current_command[1], "=") == 0 && current_command[2] != NULL)
            {
                var_set = true;

                var_name = strdup(command_name + 1);
                // printf("Var name ($var = value): %s\n",var_name);
                // Store or set the variable here
                // set_variable(var_name, var_value);

                // Skip all 3 parts of the assignment
                current_command += 2;
                curr_cmd_size -= 2;
            }
            else
            {
                fprintf(stderr, "ERROR(execute): Incorrect format\n");
                return -1;
            }

            memset(command_name,0,64);
            sscanf(current_command[0], "%63s", command_name);
        }

        
        if (strcmp(command_name, "compile") == 0) {
            ret = cmd_compile(current_command,curr_cmd_size);
            
        }
        else if (strcmp(command_name, "run") == 0) {
            ret = hnd_run();
        }
        else if (strcmp(command_name,"build") == 0)
        {
            ret = hnd_build();
        }
        else if (strcmp(command_name, "cd") == 0) {
            ret = hnd_cd(current_command,curr_cmd_size);
        }
        else if (strcmp(command_name, "cp") == 0) {
            ret = hnd_cp(current_command,command_count);
        }
        else if (strcmp(command_name, "mv") == 0) {
            ret = hnd_mv(current_command,curr_cmd_size);
        }
        else if (strcmp(command_name, "rm") == 0) {
            // Delete a file or folder
            ret = hnd_rm(current_command,curr_cmd_size);
        }
        else if (strcmp(command_name, "mkdir") == 0) {
            // Create a directory
            ret = hnd_mkdir(current_command,curr_cmd_size);
        }
        else if (strcmp(command_name, "echo") == 0) {
            // Print a message to the terminal
            // printf("echo\n");
            ret = hnd_echo(current_command,curr_cmd_size);
        }
        else if (strcmp(command_name, "exit") == 0) {
            for (size_t i = 0; i < curr_cmd_size; i++)
            {
                free(current_command[i]);
            }
            free(current_command_save);
            // Exit the script with optional code
            break;
        }
        else if (strcmp(command_name, "find") == 0) {
            // printf("IN here:%d\n",curr_cmd_size);
            // for (size_t i = 0; i < curr_cmd_size; i++)
            // {
            //     // printf("%d:%s\n",i,current_command[i]);
            // }
            // printf("Doing test alloc\n");
            // char *test_alloc = malloc(10000);
            // printf("Calling\n");
            char **values = hnd_find(current_command,curr_cmd_size);
            // printf("Call done\n");
            if(values == NULL)
            {
                printf("This sucks\n");
                ret = -1;
            }
            // printf("Outputing found files\n");
            size_t count = 0;
            
            while(values[count] != NULL)
            {
                if(values[count] == NULL)
                {
                    count++;
                    continue;
                }
                // printf("Value: %ld: %s\n",count,values[count]);
                count++;
            }
            if(var_set == true)
            {
                
                
                    // 1. Join all `values` into a single string
                    size_t total_len = 0;
                    for (size_t i = 0; values[i] != NULL; ++i)
                    {
                        total_len += strlen(values[i]) + 3;  // +1 for space or newline
                    }

                    char *joined = (char *)malloc(total_len + 10); // +1 for null terminator
                    if (!joined) {
                        fprintf(stderr, "ERROR: Failed to allocate memory for joined result.\n");
                        ret = -1;
                        return ret;
                    }

                    joined[0] = '\0';
                    for (size_t i = 0; values[i] != NULL; ++i)
                    {
                        strcat(joined, values[i]);
                        strcat(joined, " ");  // newline between files
                    }

                    // 2. Assign to variable array
                    strcpy(all_vars[num_current_vars].name, var_name);
                    // printf("Setting var %s\n",all_vars[num_current_vars].name);
                    all_vars[num_current_vars].value = joined; // already malloc'ed above
                    num_current_vars++;
                    // printf("Varset done\n");
                    var_set = false;
                    for (size_t i = 0; values[i] != NULL; ++i)
                    {
                        free(values[i]);
                    }
                    free(values);
                    free(var_name);
                
            }
            // {
            //     
            //     count++;
            // }
        }
        else if (strcmp(command_name, "env") == 0) {
            ret = hnd_env(current_command,curr_cmd_size);
        
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
            ret = hnd_wait(current_command,curr_cmd_size);
        }
        else {
            if(var_set == true)
            {
                // variable_t *new_var = (variable_t *)malloc(sizeof(variable_t));
                strcpy(all_vars[num_current_vars].name, var_name);
                all_vars[num_current_vars].value = strdup(command_name); // strdup allocates value
                num_current_vars++;
                var_set = false;
                free(var_name);
            }
            else
            {
                fprintf(stderr, "Unknown command: %s\n", command_name);
            }
            
        }

        current_command_index++;
        if(ret != 0)
        {
            current_command_index--;
            #ifdef __linux__
            printf("Failed execution at line %ld:(%s) with error %d\n",current_command_index,commands[current_command_index],ret);
            #else
            printf("Failed execution at line %lld:(%s) with error %d\n",current_command_index,commands[current_command_index],ret);
            #endif
            for (size_t i = 0; i < og_cmd_size; i++)
            {
                free(current_command_save[i]);
            }
            free(current_command_save);
            return ret;
        }
        
        

        for (size_t i = 0; i < og_cmd_size; i++)
        {
            free(current_command_save[i]);
        }
        free(current_command_save);
        
        current_command = parse_command(commands[current_command_index], &curr_cmd_size);
        current_command_save = current_command;
        og_cmd_size = curr_cmd_size;
    }
    for (size_t i = 0; i < og_cmd_size; i++)
    {
        free(current_command_save[i]);
    }
    free(current_command_save);
    return ret;
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
    printf("Executing\n");
    int exe_ret =  execute(kpm_cmd_lines,kpm_cmd_line_count);
    for (size_t i = 0; i < kpm_cmd_line_count; i++)
    {
        free(kpm_cmd_lines[i]);
    }
    free(kpm_cmd_lines);
    return exe_ret;
    
    

}