#ifndef __CUSTOM__H
#define __CUSTOM__H

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
#warning "Using _mkdir for MKDIR on Windows"
#define MKDIR(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path, mode) mkdir(path, mode)
#endif


char *fetch_json(const char *url);
#endif// 