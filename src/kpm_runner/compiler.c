#include <stdio.h>
#include "../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>


char* parse_compile_command(const char* template,
                            const char** user_flags, size_t flag_count,
                            const char* output,
                            const char** input, size_t input_count) {
    size_t buffer_size = 4096;
    char* result = (char*)malloc(buffer_size);
    if (!result) return NULL;
    result[0] = '\0';

    const char* p = template;
    while (*p) {
        if (strncmp(p, "{user_flags}", 12) == 0) {
            for (size_t i = 0; i < flag_count; ++i) {
                strncat(result, user_flags[i], buffer_size - strlen(result) - 1);
                if (i < flag_count - 1) {
                    strncat(result, " ", buffer_size - strlen(result) - 1);
                }
            }
            p += 12;
        } else if (strncmp(p, "{output}", 8) == 0) {
            strncat(result, output, buffer_size - strlen(result) - 1);
            p += 8;
        } else if (strncmp(p, "{input}", 7) == 0) {
            for (size_t i = 0; i < input_count; ++i) {
                strncat(result, input[i], buffer_size - strlen(result) - 1);
                if (i < input_count - 1) {
                    strncat(result, " ", buffer_size - strlen(result) - 1);
                }
            }
            p += 7;
        } else {
            strncat(result, (char[]){*p, '\0'}, buffer_size - strlen(result) - 1);
            p++;
        }
    }

    return result;
}

int compile_windows(char **data,char *lang)
{
    printf("Compiling with language: %s\n", lang);
    // Windows specific compilation logic
    // This is a placeholder, actual implementation will depend on the language and compiler used
    return 0;
}
const char* json_type_to_string(json_type type) {
    switch (type) {
        case JSON_OBJECT: return "object";
        case JSON_ARRAY: return "array";
        case JSON_STRING: return "string";
        case JSON_INTEGER: return "integer";
        case JSON_REAL: return "real";
        case JSON_TRUE: return "true";
        case JSON_FALSE: return "false";
        case JSON_NULL: return "null";
        default: return "unknown";
    }
}
int compile_linux(char **data,char *lang,size_t data_count)
{
    char url[1024];
    printf("Compiling with language: %s\n", lang);
    snprintf(url, sizeof(url), "%s", COMPILE_CMDS_URL);

    char *json_string = fetch_json(url);
    //Load json data
    if (!json_string) {
        fprintf(stderr, "Failed to fetch JSON data\n");
        return -1;
    }
    json_error_t error;
    json_t *json = json_loads(json_string, 0, &error);
    if (!json) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        free(json_string);
        return -1;
    }
    free(json_string);
    //Get linux command sets
    
    //Get json data for language
    json_t *lang_obj = json_object_get(json, lang);
    if (!json_is_object(lang_obj)) {
        fprintf(stderr, "Error: Language '%s' not found in JSON\n", lang);
        json_decref(json);
        return -1;
    }
    json_t *linux_lang_obj = json_object_get(lang_obj, "linux");
    if (!json_is_object(linux_lang_obj)) {
        fprintf(stderr, "Error: 'linux' is not an object in JSON\n");
        json_decref(json);
        return -1;
    }
    //Get default output
    json_t *default_output_obj = json_object_get(linux_lang_obj, "default_output");
    if (!json_is_string(default_output_obj)) {
        fprintf(stderr, "1Error: 'default_output' is not a string in language JSON\n");
        printf("Type is %s\n",json_type_to_string(json_typeof(default_output_obj)));
        json_decref(json);
        return -1;
    }
    char *output = strdup(json_string_value(default_output_obj));
    //Get compile command
    json_t *compile_cmd_obj = json_object_get(linux_lang_obj, "compile_cmd");
    
    //Get bool for single/multi command
    json_t *single_cmd_obj = json_object_get(linux_lang_obj, "single_command");
    if (!json_is_boolean(single_cmd_obj)) {
        fprintf(stderr, "Error: 'single_command' is not a boolean in language JSON\n");
        json_decref(json);
        return -1;
    }
    //Check if is single command
    if (json_is_true(single_cmd_obj)) {
        //Single command
        if (!json_is_string(compile_cmd_obj) && !json_is_null(compile_cmd_obj)) {
        fprintf(stderr, "Error: 'compile_cmd' is not a string in language JSON\n");
        printf("Type is %s\n",json_type_to_string(json_typeof(compile_cmd_obj)));
        json_decref(json);
        return -1;
        }
        if(json_is_null(compile_cmd_obj))
        {
            json_t *run_cmd_obj =  json_object_get(linux_lang_obj, "run_cmd");
            if (!json_is_string(run_cmd_obj))
            {
                 fprintf(stderr, "Error: 'run_cmd' is not a string in language JSON\n");
                printf("Type is %s\n",json_type_to_string(json_typeof(run_cmd_obj)));
                json_decref(json);
                return -1;
            }
            compile_cmd_obj = run_cmd_obj;
            char *compile_cmd = strdup(json_string_value(compile_cmd_obj));
            printf("Code is not a compiled language. Atempting to run using %s\n",compile_cmd);
            

        }
        printf("Using single command for compilation\n");
        char *compile_cmd = strdup(json_string_value(compile_cmd_obj));
        if (!compile_cmd) {
            fprintf(stderr, "Memory allocation failed for compile command\n");
            json_decref(json);
            return -1;
        }
        // printf("Comp cmd: %s\n",compile_cmd);
        //Check if compile command supports multiple files
        json_t *supports_multiple_files_obj = json_object_get(linux_lang_obj, "multi_file");
        if (!json_is_boolean(supports_multiple_files_obj)) {
            fprintf(stderr, "Error: 'multi_file' is not a boolean in language JSON\n");
            free(compile_cmd);
            json_decref(json);
            return -1;
        }
        //If it does not support multiple files, we will replace the input file in the command
      
        if(json_is_true(supports_multiple_files_obj))
        {
            //Extract input files from data, and user_flags(-flag) from data
            printf("Compile command supports multiple files\n");
            //Assuming data contains the input files and user flags
            size_t input_count = data_count; // Number of input files
            char **input_files = malloc(input_count * sizeof(char*));
            if (!input_files) {
                fprintf(stderr, "Memory allocation failed for input files\n");
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            char **user_flags = malloc(input_count * sizeof(char*));
            if (!user_flags) {
                fprintf(stderr, "Memory allocation failed for user flags\n");
                for (size_t i = 0; i < input_count; i++) {
                    free(input_files[i]);
                }
                free(input_files);
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            size_t final_file_count = 0;
            size_t user_flags_count = 0;
            for (size_t i = 0; i < input_count; i++) {
                //Check if is flag
                if (data[i][0] == '-') {
                    // It's a user flag
                    user_flags[user_flags_count] = strdup(data[i]);
                    if (!user_flags[user_flags_count]) {
                        fprintf(stderr, "Memory allocation failed for user flag %zu\n", user_flags_count);
                        for (size_t j = 0; j < user_flags_count; j++) {
                            free(user_flags[j]);
                        }
                        free(user_flags);
                        for (size_t j = 0; j < final_file_count; j++) {
                            free(input_files[j]);
                        }
                        free(input_files);
                        free(compile_cmd);
                        json_decref(json);
                        return -1;
                    }
                    user_flags_count++;
                    continue;
                    
                }
                input_files[i] = strdup(data[i]); // Copy each file name
                final_file_count++;
                //Ensure input_files[i] exists
                FILE *file = fopen(input_files[i], "r");
                if (!file) {
                    fprintf(stderr, "File %s does not exist\n", input_files[i]);
                    free(input_files[i]);
                    return -1;
                } else {
                    fclose(file);
                }
                if (!input_files[i]) {
                    fprintf(stderr, "Memory allocation failed for input file %zu\n", i);
                    for (size_t j = 0; j < i; j++) {
                        free(input_files[j]);
                    }
                    free(input_files);
                    free(compile_cmd);
                    json_decref(json);
                    return -1;
                }
            }
            //Replace files in command
            printf("Original compile command: %s\n", compile_cmd);
            //Extract output from "default_output"
            json_t *default_output_obj = json_object_get(linux_lang_obj, "default_output");
            if (!json_is_string(default_output_obj)) {
                fprintf(stderr, "Error: 'default_output' is not a string in language JSON\n");
                for (size_t i = 0; i < final_file_count; i++) {
                    free(input_files[i]);
                }
                free(input_files);
                for (size_t i = 0; i < user_flags_count; i++) {
                    free(user_flags[i]);
                }
                free(user_flags);
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            //Assuming the output file is the same for all files
            // char *default_output = strdup(json_string_value(default_output_obj));
            // if (!default_output) {
            //     fprintf(stderr, "Memory allocation failed for default output\n");
            //     for (size_t i = 0; i < final_file_count; i++) {
            //         free(input_files[i]);
            //     }
            //     free(input_files);
            //     for (size_t i = 0; i < user_flags_count; i++) {
            //         free(user_flags[i]);
            //     }
            //     free(user_flags);
            //     free(compile_cmd);
            //     json_decref(json);
            //     return -1;
            // }
            printf("Output file: [%s]\n", output);
            // char *input = data[0]; // Assuming the first file is the input
            // printf("Input file: %s\n", input);
            
            
            char *final_cmd = parse_compile_command(compile_cmd, user_flags, user_flags_count,output, input_files,final_file_count);

            if (!final_cmd) {
                fprintf(stderr, "Failed to parse compile command\n");
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            printf("Final compile command: %s\n", final_cmd);
            //Execute the command
            int result = system(final_cmd);
            if (result == -1) {
                fprintf(stderr, "Failed to execute compile command\n");
                free(final_cmd);
                for (size_t i = 0; i < final_file_count; i++) {
                    free(input_files[i]);
                }
                free(input_files);
                for (size_t i = 0; i < user_flags_count; i++) {
                    free(user_flags[i]);
                }
                free(user_flags);
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            printf("Compile command executed successfully\n");
            free(final_cmd);
            for (size_t i = 0; i < final_file_count; i++) {
                free(input_files[i]);
            }
            free(input_files);
            for (size_t i = 0; i < user_flags_count; i++) {
                free(user_flags[i]);
            }
            free(user_flags);
            // Free the compile command string
            free(compile_cmd);

        }
        else {
            //Single file command
            printf("Compile command does not support multiple files\n");
            // output = "output"; // Placeholder for output file name
            char *input = data[0]; // Assuming the first file is the input
            printf("Output file: %s\n", output);
            printf("Input file: %s\n", input);
            char *final_cmd = parse_compile_command(compile_cmd, NULL, 0, output, &input, 1);
            if (!final_cmd) {
                fprintf(stderr, "Failed to parse compile command\n");
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            printf("Final compile command: %s\n", final_cmd);
            //Execute the command
            int result = system(final_cmd);
            if (result == -1) {
                fprintf(stderr, "Failed to execute compile command\n");
                free(final_cmd);
                free(compile_cmd);
                json_decref(json);
                return -1;
            }
            printf("Compile command executed successfully\n");
            free(final_cmd);
            // Free the compile command string
            free(compile_cmd);
        }
        
    } else {
        //Multi command
        printf("Support is coming soon\n");
        printf("This code provides no output\n");
        json_t *commands = json_object_get(linux_lang_obj, "commands");
        if (!json_is_array(commands)) {
            fprintf(stderr, "Error: 'commands' is not an array in language JSON\n");
            json_decref(json);
            return -1;
        }
        size_t index;
        json_t *command;
        json_array_foreach(commands, index, command) {
            if (!json_is_string(command)) {
                fprintf(stderr, "Error: Command is not a string in language JSON\n");
                continue;
            }
            char *cmd = strdup(json_string_value(command));
            if (!cmd) {
                fprintf(stderr, "Memory allocation failed for command\n");
                continue;
            }
            //Parse command

            //Run command
            
            // int result = system(final_cmd);
            // if (result == -1) {
            //     fprintf(stderr, "Failed to execute compile command\n");
            //     free(final_cmd);
            //     free(compile_cmd);
            //     json_decref(json);
            //     return -1;
            // }

            free(cmd);
        }
    }
    //Extract run command
    json_t *run_command_object =  json_object_get(linux_lang_obj, "run_cmd");
    if (!json_is_string(run_command_object)){
        fprintf(stderr,"Error: 'run_cmd' is not a string\n");
        json_decref(linux_lang_obj);
         json_decref(json);
        return -1;

    }
    char *run_command = strdup(json_string_value(run_command_object));
    char *final_run_command = parse_compile_command(run_command,NULL,0,output,NULL,0);
    if(strcmp(output,"none") != 0)
    {
        printf("Run [%s] with command: %s\n",output,final_run_command);
    }
    
    json_decref(json);
    return 0;

}

//Takes in users file
int compile(char **data,char *lang,size_t data_count)
{
    #ifdef defined(_WIN32)
    return compile_windows(data,lang);
    #elif defined(__APPLE__)
    // macOS specific compilation logic
    printf("Compiling on macOS with language: %s\n", lang);
    // This is a placeholder, actual implementation will depend on the language and compiler used
    return 0;
    #else
    // Linux or other Unix-like systems
    printf("Compiling on Linux with language: %s\n", lang);
    return compile_linux(data,lang,data_count);
    #endif
}