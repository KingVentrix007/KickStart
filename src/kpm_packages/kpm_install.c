#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../curlhelp.h"
#include "../templates/custom.h"
#include <jansson.h>


int install_language_support(const char *lang)
{
    if (lang == NULL || strlen(lang) == 0) {
        fprintf(stderr, "Error: Language cannot be NULL or empty.\n");
        return -1;
    }
    char *url_to_fetch_from = malloc(strlen(LANG_BASE_URL) + strlen(lang) + strlen("/lang.json") + 100);

    if (url_to_fetch_from == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return -1;
    }
    snprintf(url_to_fetch_from, strlen(LANG_BASE_URL) + strlen(lang)+strlen(".json")+100, "%s/%s/lang.json", LANG_BASE_URL, lang);
    printf("Fetching JSON data from: %s\n", url_to_fetch_from);
    char *json_data = fetch_json(url_to_fetch_from);
    
    free(url_to_fetch_from);
    if (json_data == NULL) {
        fprintf(stderr, "Error: Failed to fetch JSON data for language %s.\n", lang);
        return -1;
    }
    json_error_t error;
    json_t *json = json_loads(json_data, 0, &error);
    free(json_data);
    if (!json) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return 1;
    }
    //Copy build files from the JSON object
    json_t *build_file_path = json_object_get(json, "build_file_path");
if (!json_is_object(build_file_path)) {
    fprintf(stderr, "Error: 'build_file_path' is not an object.\n");
    json_decref(json);
    return -1;
}
    size_t index;
    json_t *value;
    const char *lang_key;
json_t *lang_value;
    json_object_foreach(build_file_path, lang_key, lang_value) {
        if (!json_is_object(lang_value)) {
            fprintf(stderr, "Error: Value for '%s' is not an object.\n", lang_key);
            continue;
        }

        json_t *path = json_object_get(lang_value, "path");
        json_t *build = json_object_get(lang_value, "build");
        json_t *run = json_object_get(lang_value, "run");

        if (!json_is_string(path) || !json_is_string(build) || !json_is_string(run)) {
            fprintf(stderr, "Error: One or more fields missing or not strings in '%s'.\n", lang_key);
            continue;
        }

        const char *path_str = json_string_value(path);
        const char *build_str = json_string_value(build);
        const char *run_str = json_string_value(run);
        char *setup_path = malloc(strlen(path_str) + 1000);
        if (setup_path == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for setup_path.\n");
            json_decref(json);
            return -1;
        }
        snprintf(setup_path, strlen(path_str) + 100, "%s/%s", LANG_BASE_URL, path_str);
        // printf("Setup Path: %s\n", setup_path
        char *temp_data = fetch_data(setup_path);

        printf("Fetched data for %s: %s\n", lang_key, temp_data ? "Success" : "Failed");
        printf("Freeing setup_path\n");
        free(setup_path);
        // setup_path = NULL;
        // printf("Temp data: %s\n", temp_data ? temp_data : "NULL");
        // if (temp_data == NULL) {
        //     fprintf(stderr, "Error: Failed to fetch data for %s from %s.\n", lang_key, path_str);
        //     json_decref(json);
        //     return -1;
        // }
        printf("Freeing temp_data\n");
        if(temp_data != NULL)
        {
            free(temp_data);
        }
        
        temp_data = NULL;

        // free(setup_path);
        // free(temp_data);
        //Get main file from the JSON object: default_main_file
        

        // printf("Lang: %s\n", lang_key);
        // printf(" - Path: %s\n", path_str);
        // printf(" - Build: %s\n", build_str);
        // printf(" - Run: %s\n", run_str);

        // You can then process path_str (e.g., download or build file).
}
    printf("Build files copied successfully.\n");

    json_t *default_main_file = json_object_get(json, "default_main_file");

    if (!json_is_string(default_main_file)) {
        fprintf(stderr, "Error: 'default_main_file' is not a string.\n");
        json_decref(json);
        return -1;
    }
    const char *default_main_file_str = json_string_value(default_main_file);
    printf("Default main file: %s\n", default_main_file_str);
    //Create url from main file path
    char *main_file_url = malloc(strlen(LANG_BASE_URL) + strlen(default_main_file_str) + 100);
    if (main_file_url == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for main_file_url.\n");
        json_decref(json);  
        return -1;
    }
    snprintf(main_file_url, strlen(LANG_BASE_URL) + strlen(default_main_file_str) + 100, "%s/%s/%s", LANG_BASE_URL, lang,default_main_file_str);
    //Fetch main file data
    printf("Fetching main file data from: %s\n", main_file_url);
    char *main_file_data = fetch_data(main_file_url);
    free(main_file_url);
    if (main_file_data == NULL) {
        fprintf(stderr, "Error: Failed to fetch main file data from %s.\n", default_main_file_str);
        json_decref(json);
        return -1;
    }

}