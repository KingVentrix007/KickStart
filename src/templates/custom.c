#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "custom.h"
#include "config.h"

// Callback function to write data from curl to a buffer
size_t write_callback_json(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    char **buffer = (char **)userp;

    if (*buffer == NULL) {
        *buffer = malloc(total_size + 1);
        if (*buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory\n");
            return 0;
        }
        (*buffer)[0] = '\0';  // Ensure the buffer is null-terminated
    } else {
        *buffer = realloc(*buffer, strlen(*buffer) + total_size + 1);
        if (*buffer == NULL) {
            fprintf(stderr, "Failed to reallocate memory\n");
            return 0;
        }
    }

    memcpy(*buffer + strlen(*buffer), contents, total_size);
    (*buffer)[strlen(*buffer) + total_size] = '\0';

    return total_size;
}

// Function to fetch JSON data from a URL
char *fetch_json(const char *url) {
    CURL *curl;
    CURLcode res;
    char *buffer = NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_json);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);
        printf("URL: %s\n", url);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(buffer);
            buffer = NULL;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return buffer;
}

// Function to parse JSON and find the path for the given language
const char *find_language_path(const char *lang, const char *json_data) {
    json_error_t error;
    json_t *root;
    json_t *langs;
    json_t *lang_obj;
    json_t *path_obj;

    root = json_loads(json_data, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return NULL;
    }

    langs = json_object_get(root, "langs");
    if (!json_is_object(langs)) {
        json_decref(root);
        fprintf(stderr, "Error: 'langs' is not an object\n");
        return NULL;
    }

    lang_obj = json_object_get(langs, lang);
    if (!json_is_object(lang_obj)) {
        json_decref(root);
        fprintf(stderr, "Error: Language '%s' not found\n", lang);
        return NULL;
    }

    path_obj = json_object_get(lang_obj, "path");
    if (!json_is_string(path_obj)) {
        json_decref(root);
        fprintf(stderr, "Error: 'path' is not a string\n");
        return NULL;
    }

    const char *path = json_string_value(path_obj);
    const char *ret_path = strdup(path);
    json_decref(root);

    return ret_path;
}

// #define LANG_BASE_URL "https://raw.githubusercontent.com/KingVentrix007/CodeStarterFiles/main/langs"

char *get_lang_path(const char *lang) {
    char url[1024];
    snprintf(url, sizeof(url), "%s/index.json", LANG_BASE_URL);

    char *json_data = fetch_json(url);
    printf("json_data == %s\n",json_data);
    if (!json_data) {
        fprintf(stderr, "Failed to fetch JSON data\n");
        return NULL;
    }

    const char *path = find_language_path(lang, json_data);
    free(json_data);  // Free the JSON data after use

    if (path) {
        printf("Path for language '%s': %s\n", lang, path);
        return strdup(path);  // Return a copy of the path
    }
    return NULL;
}

// Helper macro to free allocated memory
#define FREE_STRING_ARRAY(array, count) \
    do { \
        for (size_t i = 0; i < count; i++) { \
            free(array[i]); \
        } \
        free(array); \
    } while (0)

// Structure to hold project information
typedef struct {
    char *name;
    char **system_support;
    size_t system_support_count;
    char **build_type;
    size_t build_type_count;
    int lib_support;
    char *version_template_path;
    char *build_file_path;
    char **compiler_urls;
    size_t compiler_urls_count;
    char *description;
    char *template_author;
    char *git_repo;
    char *lang_license_type;
    char *lang_license_url;
    char *default_main_file;
    char **extensions;
    size_t extensions_count;
    char **dependencies;
    size_t dependencies_count;
    char *instructions;
    char *template_version;
    char *update_url;
    char **folders_to_create;
    size_t folders_to_create_count;
    char **commands_to_run;
    size_t commands_to_run_count;
    char *main_file_path;
    char *main_file_template;
    char *comment;
} ProjectInfo;

// Function to parse JSON data into ProjectInfo structure
void parse_json(const char *json_data, ProjectInfo *info) {
    json_error_t error;

    // Load JSON data from the char* into a json_t object
    json_t *root = json_loads(json_data, 0, &error);
    if (!root) {
        fprintf(stderr, "Error loading JSON data: %s\n", error.text);
        return;
    }

    // Free previous values if they exist
    if (info->name) free(info->name);
    if (info->version_template_path) free(info->version_template_path);
    if (info->build_file_path) free(info->build_file_path);
    if (info->description) free(info->description);
    if (info->template_author) free(info->template_author);
    if (info->git_repo) free(info->git_repo);
    if (info->lang_license_type) free(info->lang_license_type);
    if (info->lang_license_url) free(info->lang_license_url);
    if (info->default_main_file) free(info->default_main_file);
    if (info->instructions) free(info->instructions);
    if (info->template_version) free(info->template_version);
    if (info->update_url) free(info->update_url);
    if (info->main_file_path) free(info->main_file_path);
    if (info->main_file_template) free(info->main_file_template);
    if (info->comment) free(info->comment);
    FREE_STRING_ARRAY(info->system_support, info->system_support_count);
    FREE_STRING_ARRAY(info->build_type, info->build_type_count);
    FREE_STRING_ARRAY(info->compiler_urls, info->compiler_urls_count);
    FREE_STRING_ARRAY(info->extensions, info->extensions_count);
    FREE_STRING_ARRAY(info->dependencies, info->dependencies_count);
    FREE_STRING_ARRAY(info->folders_to_create, info->folders_to_create_count);
    FREE_STRING_ARRAY(info->commands_to_run, info->commands_to_run_count);

    info->system_support = NULL;
    info->build_type = NULL;
    info->compiler_urls = NULL;
    info->extensions = NULL;
    info->dependencies = NULL;
    info->folders_to_create = NULL;
    info->commands_to_run = NULL;

    // Parse JSON fields
    info->name = strdup(json_string_value(json_object_get(root, "name")));

    json_t *array;
    size_t i;

    // System Support
    array = json_object_get(root, "system_support");
    info->system_support_count = json_array_size(array);
    info->system_support = malloc(info->system_support_count * sizeof(char *));
    for (i = 0; i < info->system_support_count; i++) {
        info->system_support[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Build Type
    array = json_object_get(root, "build_type");
    info->build_type_count = json_array_size(array);
    info->build_type = malloc(info->build_type_count * sizeof(char *));
    for (i = 0; i < info->build_type_count; i++) {
        info->build_type[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Lib Support
    info->lib_support = json_boolean_value(json_object_get(root, "lib_support"));

    // Version Template Path
    info->version_template_path = strdup(json_string_value(json_object_get(root, "version_template_path")));

    // Build File Path
    info->build_file_path = strdup(json_string_value(json_object_get(root, "build_file_path")));

    // Compiler URLs
    array = json_object_get(root, "compiler_urls");
    info->compiler_urls_count = json_array_size(array);
    info->compiler_urls = malloc(info->compiler_urls_count * sizeof(char *));
    for (i = 0; i < info->compiler_urls_count; i++) {
        info->compiler_urls[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Description
    info->description = strdup(json_string_value(json_object_get(root, "description")));

    // Template Author
    info->template_author = strdup(json_string_value(json_object_get(root, "template_author")));

    // Git Repo
    info->git_repo = strdup(json_string_value(json_object_get(root, "git_repo")));

    // Lang License
    json_t *lang_license = json_object_get(root, "lang_license");
    info->lang_license_type = strdup(json_string_value(json_object_get(lang_license, "type")));
    info->lang_license_url = strdup(json_string_value(json_object_get(lang_license, "url")));

    // Default Main File
    info->default_main_file = strdup(json_string_value(json_object_get(root, "default_main_file")));

    // Extensions
    array = json_object_get(root, "extensions");
    info->extensions_count = json_array_size(array);
    info->extensions = malloc(info->extensions_count * sizeof(char *));
    for (i = 0; i < info->extensions_count; i++) {
        info->extensions[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Dependencies
    array = json_object_get(root, "dependencies");
    info->dependencies_count = json_array_size(array);
    info->dependencies = malloc(info->dependencies_count * sizeof(char *));
    for (i = 0; i < info->dependencies_count; i++) {
        info->dependencies[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Instructions
    info->instructions = strdup(json_string_value(json_object_get(root, "instructions")));

    // Template Version
    info->template_version = strdup(json_string_value(json_object_get(root, "template_version")));

    // Update URL
    info->update_url = strdup(json_string_value(json_object_get(root, "update_url")));

    // Folders to Create
    array = json_object_get(root, "folders_to_create");
    info->folders_to_create_count = json_array_size(array);
    info->folders_to_create = malloc(info->folders_to_create_count * sizeof(char *));
    for (i = 0; i < info->folders_to_create_count; i++) {
        info->folders_to_create[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Commands to Run
    array = json_object_get(root, "commands_to_run");
    info->commands_to_run_count = json_array_size(array);
    info->commands_to_run = malloc(info->commands_to_run_count * sizeof(char *));
    for (i = 0; i < info->commands_to_run_count; i++) {
        info->commands_to_run[i] = strdup(json_string_value(json_array_get(array, i)));
    }

    // Main File Path
    info->main_file_path = strdup(json_string_value(json_object_get(root, "main_file_path")));

    // Main File Template
    info->main_file_template = strdup(json_string_value(json_object_get(root, "main_file_template")));

    // Comment
    info->comment = strdup(json_string_value(json_object_get(root, "comment")));

    json_decref(root);
}

// Function to create a project
int create_project(char *project_name, char *project_description, char *project_author, char *project_licence, char *project_version, char *project_language, char *project_dependencies, char *generate_readme, char *initialize_git, char *create_license_file, char *generate_structure) {
    const char *base_dir = "tests";
    #ifndef DEBUG
    base_dir = ".";
    #endif
    if (strcmp(base_dir, "tests") == 0) {
        system("mkdir -p tests");
    }

    char *lang_path = get_lang_path(project_language);
    if (lang_path == NULL) {
        fprintf(stderr, "Failed to get path for language '%s'\n", project_language);
        return 1;
    }
    printf("Language path: %s\n", lang_path);

    char lang_json[1024];
    snprintf(lang_json, sizeof(lang_json), "%s%s", LANG_BASE_URL, lang_path);
    free(lang_path);  // Free lang_path after use

    char *lang_json_data = fetch_json(lang_json);
    if (!lang_json_data) {
        fprintf(stderr, "Failed to fetch language JSON data\n");
        return 1;
    }

    // Initialize ProjectInfo structure
    ProjectInfo info;
    printf("lang_json_data == %s\n",lang_json_data);
    memset(&info, 0, sizeof(info));
    parse_json(lang_json_data, &info);
    free(lang_json_data);  // Free lang_json_data after use

    // Use the ProjectInfo structure for further project creation tasks...

    return 0;
}
