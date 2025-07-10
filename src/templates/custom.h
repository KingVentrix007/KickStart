#ifndef __CUSTOM__H
#define __CUSTOM__H
#include "stdbool.h"
#define LANG_BASE_URL "https://raw.githubusercontent.com/KingVentrix007/KickStartFiles/main/langs"
#define COMPILE_CMDS_URL "https://raw.githubusercontent.com/KingVentrix007/KickStartFiles/main/compile_cmds.json"
#define HASH_URL "https://raw.githubusercontent.com/{owner}/{repo}/main/{path}"
#define LINUX_STORE_PATH "local/share/KickStart/langs"
#define WINDOWS_STORE_PATH "AppData/Local/KickStart/langs"
#define MAC_STORE_PATH "Library/Application Support/KickStart/langs"

#if defined(_WIN32)
    #define STORE_PATH WINDOWS_STORE_PATH
#elif defined(__APPLE__)
    #define STORE_PATH MAC_STORE_PATH
#elif defined(__linux__)
    #define STORE_PATH LINUX_STORE_PATH
#else
    #error "Unsupported OS"
#endif

#if defined(_WIN32)
    #define HOME_ENV_NAME "USERPROFILE"  // Windows home directory
#elif defined(__APPLE__) || defined(__linux__)
    #define HOME_ENV_NAME "HOME"         // Unix-like systems
#else
    #error "Unsupported OS"
#endif

#ifdef _WIN32
#include <direct.h>
// #warning "Using _mkdir for MKDIR on Windows"
#define MKDIR(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path, mode) mkdir(path, mode)
#endif
typedef struct {
    char *makefile_path;
    char *bash_path;
} BuildFilePaths;

typedef struct{
    char *name;
    char *path;
    char *build_command;
    char *run_command;
    bool create_file;
}BuildSystem;
typedef struct {
    char *name;
    int version;
    char **system_support;
    size_t system_support_count;
    char **build_type;
    // size_t build_type_count; 
    int lib_support;
    bool special_build; // Only present in version 2
    BuildFilePaths build_file_paths;  //Might not be present in version 2
    BuildSystem *build_systems;
    size_t build_systems_count;
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
    char *compiler_cmd; 
    char *package_install_command;
    bool create_file;
} ProjectInfo;


char *fetch_json(const char *url);
#endif// 