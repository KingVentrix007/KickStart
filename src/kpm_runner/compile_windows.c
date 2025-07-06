#include <stdio.h>
#include "../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "compile_windows.h"
#include "compile_utils.h"

static int validate_file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    fclose(file);
    return 1;
}

static int extract_compile_data(json_t *win_lang_obj, char ***input_files, char ***user_flags, size_t *file_count, size_t *flag_count, char **output, char **compile_cmd, char **final_cmd, char **data, size_t data_count) {
    json_t *default_output_obj = json_object_get(win_lang_obj, "default_output");
    if (!json_is_string(default_output_obj)) return -1;
    *output = strdup(json_string_value(default_output_obj));

    json_t *compile_cmd_obj = json_object_get(win_lang_obj, "compile_cmd");
    if (!json_is_string(compile_cmd_obj)) return -1;
    *compile_cmd = strdup(json_string_value(compile_cmd_obj));

    *input_files = malloc(data_count * sizeof(char*));
    *user_flags = malloc(data_count * sizeof(char*));
    if (!(*input_files) || !(*user_flags)) return -1;

    *file_count = 0;
    *flag_count = 0;
    for (size_t i = 0; i < data_count; i++) {
        if (data[i][0] == '-') {
            (*user_flags)[*flag_count] = strdup(data[i]);
            (*flag_count)++;
        } else {
            if (!validate_file_exists(data[i])) return -1;
            (*input_files)[*file_count] = strdup(data[i]);
            (*file_count)++;
        }
    }

    *final_cmd = parse_compile_command(*compile_cmd, *user_flags, *flag_count, *output, *input_files, *file_count);
    return *final_cmd ? 0 : -1;
}

static int run_single_file_compile(json_t *win_lang_obj, char *input, char *output) {
    json_t *compile_cmd_obj = json_object_get(win_lang_obj, "compile_cmd");
    if (!json_is_string(compile_cmd_obj)) return -1;
    char *compile_cmd = strdup(json_string_value(compile_cmd_obj));
    char *final_cmd = parse_compile_command(compile_cmd, NULL, 0, output, &input, 1);
    int result = system(final_cmd);
    free(compile_cmd);
    free(final_cmd);
    return result;
}

static int run_multi_command(json_t *win_lang_obj) {
    json_t *commands = json_object_get(win_lang_obj, "commands");
    if (!json_is_array(commands)) return -1;

    size_t index;
    json_t *command;
    json_array_foreach(commands, index, command) {
        if (!json_is_string(command)) continue;
        char *cmd = strdup(json_string_value(command));
        printf("Executing command %s\n", cmd);
        system(cmd);
        free(cmd);
    }
    return 0;
}

static void run_output_program(json_t *win_lang_obj, const char *output) {
    json_t *run_cmd_obj = json_object_get(win_lang_obj, "run_cmd");
    if (!json_is_string(run_cmd_obj)) return;
    char *run_cmd = strdup(json_string_value(run_cmd_obj));
    char *final_run = parse_compile_command(run_cmd, NULL, 0, (char*)output, NULL, 0);
    if (strcmp(output, "none") != 0) {
        printf("Run [%s] with command: %s\n", output, final_run);
        system(final_run);
    }
    free(run_cmd);
    free(final_run);
}

int compile_windows(char **data, char *lang, size_t data_count) {
    printf("Compiling (Windows) with language: %s\n", lang);

    char *json_string = fetch_json(COMPILE_CMDS_URL);
    if (!json_string) return -1;

    json_error_t error;
    json_t *json = json_loads(json_string, 0, &error);
    free(json_string);
    if (!json) return -1;

    json_t *lang_obj = json_object_get(json, lang);
    if (!json_is_object(lang_obj)) return -1;

    json_t *win_lang_obj = json_object_get(lang_obj, "windows");
    if (!json_is_object(win_lang_obj)) return -1;

    json_t *single_cmd_obj = json_object_get(win_lang_obj, "single_command");
    if (!json_is_boolean(single_cmd_obj)) return -1;

    if (json_is_true(single_cmd_obj)) {
        json_t *multi_file_obj = json_object_get(win_lang_obj, "multi_file");
        if (json_is_true(multi_file_obj)) {
            char **input_files = NULL, **user_flags = NULL;
            char *output = NULL, *compile_cmd = NULL, *final_cmd = NULL;
            size_t file_count = 0, flag_count = 0;
            if (extract_compile_data(win_lang_obj, &input_files, &user_flags, &file_count, &flag_count, &output, &compile_cmd, &final_cmd, data, data_count) != 0) return -1;
            int result = system(final_cmd);
            free(output); free(compile_cmd); free(final_cmd);
            for (size_t i = 0; i < file_count; i++) free(input_files[i]);
            for (size_t i = 0; i < flag_count; i++) free(user_flags[i]);
            free(input_files); free(user_flags);
            if (result == -1) return -1;
        } else {
            char *output = NULL;
            json_t *default_output_obj = json_object_get(win_lang_obj, "default_output");
            if (!json_is_string(default_output_obj)) return -1;
            output = strdup(json_string_value(default_output_obj));
            if (run_single_file_compile(win_lang_obj, data[0], output) == -1) return -1;
            free(output);
        }
    } else {
        if (run_multi_command(win_lang_obj) != 0) return -1;
    }

    json_t *default_output_obj = json_object_get(win_lang_obj, "default_output");
    const char *output = json_is_string(default_output_obj) ? json_string_value(default_output_obj) : "none";
    run_output_program(win_lang_obj, output);

    json_decref(json);
    return 0;
}
