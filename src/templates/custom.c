#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "custom.h"
// #include "config.h"
#include "../curlhelp.h"
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "utils.h"
#include "../licence.h"
#include "limits.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};
// Function to replace ${project_name} with the actual project name
char *replace_placeholder(const char *path, const char *project_name) {
    const char *placeholder = "${project_name}";
    char *result;
    char *pos;

    // Find the position of the placeholder in the path
    pos = strstr(path, placeholder);
    if (pos == NULL) {
        // If the placeholder is not found, return a copy of the original path
        return strdup(path);
    }

    // Allocate memory for the new path
    result = malloc(strlen(path) - strlen(placeholder) + strlen(project_name) + 1);
    if (!result) {
        perror("Error allocating memory");
        return NULL;
    }

    // Copy the part of the path before the placeholder
    strncpy(result, path, pos - path);
    result[pos - path] = '\0';

    // Append the project name and the rest of the path
    strcat(result, project_name);
    strcat(result, pos + strlen(placeholder));

    return result;
}
int create_directories(const char *full_path) {
    char temp_path[4096];
    snprintf(temp_path, sizeof(temp_path), "%s", full_path);

    for (char *p = temp_path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';

            // Check if directory exists, if not, create it
            if (access(temp_path, F_OK) != 0) {
                if (mkdir(temp_path, 0755) != 0) {
                    perror("Error creating folder");
                    return 1;
                }
            }

            *p = '/';
        }
    }

    // Finally, create the last directory in the path if it does not exist
    if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
        perror("Error creating folder");
        return 1;
    }

    return 0;
}
// Callback function to write data from curl to a buffer
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        // Out of memory!
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to fetch JSON data from a URL
char *fetch_json(const char *url) {
    CURL *curl;
    CURLcode res;
    char *buffer = NULL;
    long http_code = 0;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);
        printf("URL: %s\n", url);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(buffer);
            buffer = NULL;
        }
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            fprintf(stderr, "Failed to fetch data. HTTP response code: %ld\n", http_code);
        }
        
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    printf("buffer == [%s]\n",buffer);
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

// #define LANG_BASE_URL "https://raw.githubusercontent.com/KingVentrix007/KickStartFiles/main/langs"

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
typedef struct {
    char *makefile_path;
    char *bash_path;
} BuildFilePaths;

// Structure to hold project information
typedef struct {
    char *name;
    int version;
    char **system_support;
    size_t system_support_count;
    char **build_type;
    size_t build_type_count; 
    int lib_support;
    BuildFilePaths build_file_paths;  //Might not be present in version 2
    char *git_ignore_path;            
    char *version_template_path;
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
     char **compiler_urls;
    size_t compiler_urls_count;
    char **files_to_include; // Only in version 2
    size_t files_to_include_count; // Only in version 2
} ProjectInfo;


// Function to parse JSON data into ProjectInfo structure
//! Check this function for Segmentation fault, might be deref error
//! Or might be a memory leak, missed ptr, etc
#define LOG_LOCATION //printf("%s:%d\n", __FILE__, __LINE__);
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
    if (info->build_file_paths.makefile_path) free(info->build_file_paths.makefile_path);
    if (info->build_file_paths.bash_path) free(info->build_file_paths.bash_path);
    if (info->git_ignore_path) free(info->git_ignore_path);
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
    FREE_STRING_ARRAY(info->extensions, info->extensions_count);
    FREE_STRING_ARRAY(info->dependencies, info->dependencies_count);
    FREE_STRING_ARRAY(info->folders_to_create, info->folders_to_create_count);
    FREE_STRING_ARRAY(info->commands_to_run, info->commands_to_run_count);
    FREE_STRING_ARRAY(info->compiler_urls, info->compiler_urls_count);
    FREE_STRING_ARRAY(info->files_to_include, info->files_to_include_count); // Only free in version 2

    // Parsing the JSON data into the ProjectInfo structure
    json_t *project_name = json_object_get(root, "name");
    json_t *version = json_object_get(root, "version"); // Assuming version is an integer now
    json_t *system_support = json_object_get(root, "system_support");
    json_t *build_type = json_object_get(root, "build_type");
    json_t *lib_support = json_object_get(root, "lib_support");
    // json_t *build_file_paths = json_object_get(root, "build_file_paths");
    json_t *git_ignore_path = json_object_get(root, "git_ignore_path");
    json_t *version_template_path = json_object_get(root, "version_template_path");
    json_t *description = json_object_get(root, "description");
    json_t *template_author = json_object_get(root, "template_author");
    json_t *git_repo = json_object_get(root, "git_repo");
    json_t *lang_license_type = json_object_get(root, "lang_license_type");
    json_t *lang_license_url = json_object_get(root, "lang_license_url");
    json_t *default_main_file = json_object_get(root, "default_main_file");
    json_t *extensions = json_object_get(root, "extensions");
    json_t *dependencies = json_object_get(root, "dependencies");
    json_t *instructions = json_object_get(root, "instructions");
    json_t *template_version = json_object_get(root, "template_version");
    json_t *update_url = json_object_get(root, "update_url");
    json_t *folders_to_create = json_object_get(root, "folders_to_create");
    json_t *commands_to_run = json_object_get(root, "commands_to_run");
    json_t *main_file_path = json_object_get(root, "main_file_path");
    json_t *main_file_template = json_object_get(root, "main_file_template");
    json_t *comment = json_object_get(root, "comment");
    json_t *compiler_urls = json_object_get(root, "compiler_urls");
    json_t *files_to_include = json_object_get(root, "files_to_include"); // Only for version 2

    // Set default values or parse the values from JSON
    info->version = json_is_integer(version) ? json_integer_value(version) : 1;
    info->name = project_name && json_is_string(project_name) ? strdup(json_string_value(project_name)) : NULL;
    info->lib_support = json_is_integer(lib_support) ? json_integer_value(lib_support) : 0;
    info->git_ignore_path = git_ignore_path && json_is_string(git_ignore_path) ? strdup(json_string_value(git_ignore_path)) : NULL;
    info->version_template_path = version_template_path && json_is_string(version_template_path) ? strdup(json_string_value(version_template_path)) : NULL;
    info->description = description && json_is_string(description) ? strdup(json_string_value(description)) : NULL;
    info->template_author = template_author && json_is_string(template_author) ? strdup(json_string_value(template_author)) : NULL;
    info->git_repo = git_repo && json_is_string(git_repo) ? strdup(json_string_value(git_repo)) : NULL;
    info->lang_license_type = lang_license_type && json_is_string(lang_license_type) ? strdup(json_string_value(lang_license_type)) : NULL;
    info->lang_license_url = lang_license_url && json_is_string(lang_license_url) ? strdup(json_string_value(lang_license_url)) : NULL;
    info->default_main_file = default_main_file && json_is_string(default_main_file) ? strdup(json_string_value(default_main_file)) : NULL;
    info->instructions = instructions && json_is_string(instructions) ? strdup(json_string_value(instructions)) : NULL;
    info->template_version = template_version && json_is_string(template_version) ? strdup(json_string_value(template_version)) : NULL;
    info->update_url = update_url && json_is_string(update_url) ? strdup(json_string_value(update_url)) : NULL;
    info->main_file_path = main_file_path && json_is_string(main_file_path) ? strdup(json_string_value(main_file_path)) : NULL;
    info->main_file_template = main_file_template && json_is_string(main_file_template) ? strdup(json_string_value(main_file_template)) : NULL;
    info->comment = comment && json_is_string(comment) ? strdup(json_string_value(comment)) : NULL;

    // If version is 1, process build_file_paths
    if (1 == 1) {
        // Updated Build File Paths
        json_t *build_file_path = json_object_get(root, "build_file_path");
        info->build_file_paths.makefile_path = strdup(json_string_value(json_object_get(build_file_path, "makefile")));
        info->build_file_paths.bash_path = strdup(json_string_value(json_object_get(build_file_path, "bash")));
    }

    // Parse arrays for system_support, build_type, extensions, dependencies, folders_to_create, commands_to_run, compiler_urls
    info->system_support_count = json_is_array(system_support) ? json_array_size(system_support) : 0;
    info->system_support = info->system_support_count > 0 ? malloc(info->system_support_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->system_support_count; i++) {
        info->system_support[i] = strdup(json_string_value(json_array_get(system_support, i)));
    }

    info->build_type_count = json_is_array(build_type) ? json_array_size(build_type) : 0;
    info->build_type = info->build_type_count > 0 ? malloc(info->build_type_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->build_type_count; i++) {
        info->build_type[i] = strdup(json_string_value(json_array_get(build_type, i)));
    }

    info->extensions_count = json_is_array(extensions) ? json_array_size(extensions) : 0;
    info->extensions = info->extensions_count > 0 ? malloc(info->extensions_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->extensions_count; i++) {
        info->extensions[i] = strdup(json_string_value(json_array_get(extensions, i)));
    }

    info->dependencies_count = json_is_array(dependencies) ? json_array_size(dependencies) : 0;
    info->dependencies = info->dependencies_count > 0 ? malloc(info->dependencies_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->dependencies_count; i++) {
        info->dependencies[i] = strdup(json_string_value(json_array_get(dependencies, i)));
    }

    info->folders_to_create_count = json_is_array(folders_to_create) ? json_array_size(folders_to_create) : 0;
    info->folders_to_create = info->folders_to_create_count > 0 ? malloc(info->folders_to_create_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->folders_to_create_count; i++) {
        info->folders_to_create[i] = strdup(json_string_value(json_array_get(folders_to_create, i)));
    }

    info->commands_to_run_count = json_is_array(commands_to_run) ? json_array_size(commands_to_run) : 0;
    info->commands_to_run = info->commands_to_run_count > 0 ? malloc(info->commands_to_run_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->commands_to_run_count; i++) {
        info->commands_to_run[i] = strdup(json_string_value(json_array_get(commands_to_run, i)));
    }

    info->compiler_urls_count = json_is_array(compiler_urls) ? json_array_size(compiler_urls) : 0;
    info->compiler_urls = info->compiler_urls_count > 0 ? malloc(info->compiler_urls_count * sizeof(char *)) : NULL;
    for (size_t i = 0; i < info->compiler_urls_count; i++) {
        info->compiler_urls[i] = strdup(json_string_value(json_array_get(compiler_urls, i)));
    }

    if (info->version == 2) {
        info->files_to_include_count = json_is_array(files_to_include) ? json_array_size(files_to_include) : 0;
        info->files_to_include = info->files_to_include_count > 0 ? malloc(info->files_to_include_count * sizeof(char *)) : NULL;
        for (size_t i = 0; i < info->files_to_include_count; i++) {
            info->files_to_include[i] = strdup(json_string_value(json_array_get(files_to_include, i)));
        }
    }

    // Return the parsed information
    // return info;
}
void clean_url(char *url) {
    size_t len = strlen(url);

    // Remove trailing '%' if it exists
    if (len > 0 && url[len - 1] == '%') {
        url[len - 1] = '\0';
        len--;
    }

    // Remove trailing '//' if it exists
    if (len > 1 && url[len - 1] == '/' && url[len - 2] == '/') {
        url[len - 2] = '\0';
    }
}
// Function to create a project
int create_project(char *project_name, char *project_description, char *project_author, char *project_licence, char *project_version, char *project_language, char *project_dependencies, char *generate_readme, char *initialize_git, char *create_license_file) {
    const char *base_dir = ".";
    // char *temp = generate_structure;
    // memset(temp,0,1);
    #ifndef DEBUG
    base_dir = ".";
    #endif
    // if (strcmp(base_dir, "tests") == 0) {
        // system("mkdir -p tests");
    // }
    // system("mkdir -p tests/tests");
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
    printf("lang_json_data == [%s]\n",lang_json_data);
    memset(&info, 0, sizeof(info));
    parse_json(lang_json_data, &info);
    free(lang_json_data);  // Free lang_json_data after use
    // Create project directorys
    for (size_t i = 0; i < info.folders_to_create_count; i++) {
        char *folder_path = replace_placeholder(info.folders_to_create[i], project_name);
        if (!folder_path) {
            fprintf(stderr, "Failed to create folder path\n");
            return 1;
        }

        char *full_path = malloc(strlen(base_dir) + strlen(folder_path) + 2);
        if (!full_path) {
            perror("Error allocating memory for full path");
            free(folder_path);
            return 1;
        }

        sprintf(full_path, "%s/%s", base_dir, folder_path);
        printf("Creating folder: %s\n", full_path);

        if (create_directories(full_path) != 0) {
            free(folder_path);
            free(full_path);
            return 1;
        }

        free(folder_path);
        free(full_path);
    }

    // ds
    if(system("make -v") == 0)
    {
        char *makefile_path = malloc(strlen(LANG_BASE_URL) + strlen(info.build_file_paths.makefile_path) + 10);

        if (makefile_path == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return 1;
        }

        // Format the string safely using snprintf
        printf("Here\n");
        snprintf(makefile_path, strlen(LANG_BASE_URL) + strlen(info.build_file_paths.makefile_path) + 10, "%s/%s", LANG_BASE_URL, info.build_file_paths.makefile_path);

        // Output the final path
        // printf("Makefile Path: %s\n", makefile_path);
        // clean_url(makefile_path);
        char *makefile_data = fetch_data(makefile_path);
        // clean_url(makefile_path);
        printf("%s\n",makefile_data);
        char makefile_create_path[1024];
        char *makefile =  "makefile";
        snprintf(makefile_create_path, sizeof(makefile_create_path), "%s/%s", base_dir,makefile);
        FILE *fp = fopen(makefile_create_path,"w");
        fwrite(makefile_data, 1, strlen(makefile_data), fp);
        fclose(fp);
        // free(makefile_create_path);
        free(makefile_path);
        free(makefile_data);
            char *config_mk_path = malloc(strlen(LANG_BASE_URL) + strlen("config.mk") + 12);
        if (config_mk_path == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return 1;
        }
        // Format the string safely using snprintf
        snprintf(config_mk_path, strlen(LANG_BASE_URL) + strlen("config.mk") + 12, "%s/%s/%s", LANG_BASE_URL, project_language,"config.mk");
        char *config_mk_data = fetch_data(config_mk_path);
        char config_mk_create_path[1024];
        snprintf(config_mk_create_path, sizeof(config_mk_create_path), "%s/%s", base_dir, "config.mk");
        // char *config_mk_create_path_formatted = replace_string(config_mk_create_path, "${project_name}", project_name);
        char *config_mk_data_formatted = replace_string(config_mk_data, "${project_name}", project_name);
        FILE *fp3 = fopen(config_mk_create_path,"w");
        fwrite(config_mk_data_formatted, 1, strlen(config_mk_data_formatted), fp3);
        fclose(fp3);
        free(config_mk_path);
        free(config_mk_data);
        free(config_mk_data_formatted);
    }
    else
    {
        printf("make is not installed\n");
        printf("It is recommended to install make to build your project, but a bash script will be used i ints place\n");
        char *bash_script_path = malloc(strlen(info.build_file_paths.bash_path)+20);
        if (bash_script_path == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return 1;
        }
        // Format the string safely using snprintf
        snprintf(bash_script_path, strlen(LANG_BASE_URL) + strlen(info.build_file_paths.bash_path) + 10, "%s/%s", LANG_BASE_URL, info.build_file_paths.bash_path);
        char *bash_script_data = fetch_data(bash_script_path);
        char bash_script_create_path[1024];
        snprintf(bash_script_create_path, sizeof(bash_script_create_path), "%s/%s", base_dir, "build.sh");
        char *bash_script_data_formatted = replace_string(bash_script_data, "${project_name}", project_name);
        FILE *fp5 = fopen(bash_script_create_path,"w");
        fwrite(bash_script_data_formatted, 1, strlen(bash_script_data_formatted), fp5);
        fclose(fp5);
        free(bash_script_path);
        free(bash_script_data);


    }
    // Create main.
    char *main_file_path = malloc(strlen(LANG_BASE_URL) + strlen(info.main_file_template) + 10);
    if (main_file_path == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    // Format the string safely using snprintf
    snprintf(main_file_path, strlen(LANG_BASE_URL) + strlen(info.main_file_template) + 10, "%s/%s", LANG_BASE_URL, info.main_file_template);
    printf("main_file_path == %s\n",main_file_path);
    char *main_file_data = fetch_data(main_file_path);
    char main_file_create_path[1024];
    snprintf(main_file_create_path, sizeof(main_file_create_path), "%s/%s", base_dir,info.main_file_path);
    char *formatted_main_file_path = replace_string(main_file_create_path, "${project_name}", project_name);
    printf("main_file_create_path == %s\n",formatted_main_file_path);
    FILE *fp2 = fopen(formatted_main_file_path,"w");
    fprintf(fp2, "%s File: %s\n",info.comment,info.default_main_file);
    fprintf(fp2, "%s Author: %s\n",info.comment, project_author);
    fprintf(fp2, "%s License: %s\n",info.comment, project_licence);
    fprintf(fp2, "%s Version: %s\n", info.comment,project_version);
    fprintf(fp2, "%s Description: %s\n\n", info.comment,project_description);
    fwrite(main_file_data, 1, strlen(main_file_data), fp2);
    fclose(fp2);
    free(main_file_path);
    free(main_file_data);
    // chdir(base_dir);
    //Run commnads
    for (size_t i = 0; i < info.commands_to_run_count; i++) {
        char *command = replace_string(info.commands_to_run[i], "${project_name}", project_name);
        // printf("Running command: %s\n", command);
        int result = system(command);
        free(command);
        if (result != 0) {
            fprintf(stderr, "Command execution failed\n");
            // return 1;
            }
    }
    // if (strcmp(base_dir, "tests") == 0) {
    //     // system("mkdir -p tests");
    //     chdir("..");
    // }
    //Config.mk
   
    // free(config_mk_create_path_formatted);
    // Create README.md
    if (strcmp(create_license_file, "yes") == 0) {
        char license_file_path[1024];
        snprintf(license_file_path, sizeof(license_file_path), "%s/LICENSE", base_dir);
        FILE *license_file = fopen(license_file_path, "w");
        if (license_file == NULL) {
            perror("Error creating LICENSE");
            exit(EXIT_FAILURE);
        }
        char *license_file_content = get_license(project_licence);
        fprintf(license_file, "%s", license_file_content);
        free(license_file_content);
        fclose(license_file);
    }
    if (strcmp(generate_readme, "yes") == 0) {
        char readme_file_path[1024];
        snprintf(readme_file_path, sizeof(readme_file_path), "%s/README.md", base_dir);
        FILE *readme_file = fopen(readme_file_path, "w");
        if (readme_file == NULL) {
            perror("Error creating README.md");
            exit(EXIT_FAILURE);
        }
        fprintf(readme_file, "# %s\n\n", project_name);
        fprintf(readme_file, "%s\n\n", project_description);
        fclose(readme_file);
    }
    if (strcmp(initialize_git, "yes") == 0) {
        if (system("git --version") != 0) {
            printf("Git is not installed. Download Git from https://git-scm.com/downloads\n");
        } else {
            char git_init_cmd[1024];
            snprintf(git_init_cmd, sizeof(git_init_cmd), "cd %s && git init", base_dir);
            system(git_init_cmd);
            system("git add .");
            system("git commit -m \"Initial commit\"");
        }
        char *gitignore_path = malloc(strlen(info.git_ignore_path)+10);
        if (gitignore_path == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return 1;
        }
        char *gitignore_data = fetch_data(gitignore_path);
        char gitignore_create_path[1024];
        snprintf(gitignore_create_path, sizeof(gitignore_create_path), "%s/%s", base_dir, ".gitignore");
        // char *config_mk_create_path_formatted = replace_string(config_mk_create_path, "${project_name}", project_name);
        char *gitignore_data_formatted = replace_string(gitignore_data, "${project_name}", project_name);
        FILE *fp4 = fopen(gitignore_create_path,"w");
        fwrite(gitignore_data_formatted, 1, strlen(gitignore_data_formatted), fp4);
        fclose(fp4);
        
    }
    char project_json_path[1024];
    snprintf(project_json_path, sizeof(project_json_path), "%s/project.json", base_dir);
    FILE *project_json = fopen(project_json_path, "w");
    if (project_json == NULL) {
        perror("Error creating project.json");
        exit(EXIT_FAILURE);
        
    }
    char project_name_copy[strlen(project_name) + 1];
    strcpy(project_name_copy, project_name);

    char project_version_copy[strlen(project_version) + 1];
    strcpy(project_version_copy, project_version);

    char project_description_copy[strlen(project_description) + 1];
    strcpy(project_description_copy, project_description);

    char project_author_copy[strlen(project_author) + 1];
    strcpy(project_author_copy, project_author);

    char project_license_copy[strlen(project_licence) + 1];
    strcpy(project_license_copy, project_licence);

    char project_dependencies_copy[strlen(project_dependencies) + 1];
    strcpy(project_dependencies_copy, project_dependencies);

    remove_trailing_newline(project_name_copy);
    remove_trailing_newline(project_version_copy);
    remove_trailing_newline(project_description_copy);
    remove_trailing_newline(project_author_copy);
    remove_trailing_newline(project_license_copy);
    remove_trailing_newline(project_dependencies_copy);

    fprintf(project_json, "{\n");
    fprintf(project_json, "    \"name\": \"%s\",\n", project_name_copy);
    fprintf(project_json, "    \"version\": \"%s\",\n", project_version_copy);
    fprintf(project_json, "    \"description\": \"%s\",\n", project_description_copy);
    fprintf(project_json, "    \"author\": \"%s\",\n", project_author_copy);
    fprintf(project_json, "    \"license\": \"%s\",\n", project_license_copy);
    fprintf(project_json, "    \"dependencies\": \"%s\",\n", project_dependencies_copy);
    fprintf(project_json, "    \"lang\": \"%s\"\n",project_language);
    fprintf(project_json, "}\n");
    fclose(project_json);

        // char *readme_data = fetch_data(readme_path);
    // Use the ProjectInfo structure for further project creation tasks...

    return 0;
}
