#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "jansson.h"
#include "package_manager/cpkg_main.h"
#include "run/run.h"
#include "language.h"
#include "templates/custom.h"

#ifdef _WIN32
#warning "This program does not support Windows yet. Use at your own risk"
#endif

// Global offline mode flag
static bool offline = false;

// Function declarations
int ignore(char **ignore_list, size_t count);
int get_line_count();
bool is_offline() { return offline; }
int create_template();
char* get_lang();
char* get_install();
int main_build();
int update_deps(char *installed);

// Command function prototypes
int cmd_init(int argc, char **argv);
int cmd_template(int argc, char **argv);
int cmd_install(int argc, char **argv);
int cmd_run(int argc, char **argv);
int cmd_build(int argc, char **argv);
int cmd_langs(int argc, char **argv);
int cmd_ignore(int argc, char **argv);
int cmd_count(int argc, char **argv);

// Command structure
typedef struct {
    const char *name;
    int (*handler)(int argc, char **argv);
    const char *description;
} Command;

// Command implementations
int cmd_init(int argc, char **argv) {
#ifdef _WIN32
    printf("WARNING: Running on Windows. kpm commands are Linux-focused.\n");
#endif
    return main_build();
}

int cmd_template(int argc, char **argv) {
    if (argc >= 3 && strcmp(argv[2], "-f") == 0) {
        printf("Feature not implemented yet: auto-generate template from folder.\n");
    } else {
        printf("WARNING: This command is deprecated with the latest template system.\n");
        return create_template();
    }
    return 0;
}


int cmd_install(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s install <package_name>\n", argv[0]);
        return 1;
    }

    printf("NOTE: This package manager has limited package support.\n");

    char *input = argv[2];
    char *lang = NULL;
    char *install_cmd = NULL;

    // Check if input contains '@'
    char *at_sign = strchr(input, '@');
    char *package_name = input;

    if (at_sign != NULL) {
        // Split into lang and package parts
        size_t lang_len = at_sign - input;

        // Extract language string before '@'
        char lang_buf[64];
        if (lang_len >= sizeof(lang_buf)) {
            fprintf(stderr, "Language string too long.\n");
            return 1;
        }
        strncpy(lang_buf, input, lang_len);
        lang_buf[lang_len] = '\0';

        // Extract package name after '@'
        package_name = at_sign + 1;

        // Override lang with extracted lang
        lang = lang_buf;

        // If lang is "kpm", call install_lang_support with package_name as language
        if (strcmp(lang, "kpm") == 0) {
            install_lang_support_internal(package_name);
            return 0;
        }
        else
        {
            FILE *cf = fopen("project.json", "r");
            if(cf != NULL)
            {
                fclose(cf);
                char *lang_int = get_lang();
                if(strcmp(lang, lang_int) != 0)
                {
                    printf("WARNING: Installing package for language '%s', while project is in '%s'\nThese languages might not be compatible",lang,lang_int);
                    
                }
                // else
                // {
                //     printf("Using language '%s' for package '%s'.\n", lang, package_name);
                // }
                
            }
            printf("Using language '%s' for package '%s'.\n", lang, package_name);
            // printf("This is not recommended, use 'kpm install <package_name>' to install packages.\n");
            char *url_for_install_cmd = malloc(strlen(LANG_BASE_URL) + strlen(lang) + strlen("/lang.json") + 100);
            if (url_for_install_cmd == NULL) {
                fprintf(stderr, "Memory allocation failed for URL.\n");
                return 1;
            }
            snprintf(url_for_install_cmd, strlen(LANG_BASE_URL) + strlen(lang) + strlen("/lang.json") + 100, "%s/%s/lang.json", LANG_BASE_URL, lang);
            printf("Fetching install command from: %s\n", url_for_install_cmd);
            char *lang_json_data = fetch_json(url_for_install_cmd);
            free(url_for_install_cmd);
            if (lang_json_data == NULL) {
                fprintf(stderr, "Failed to fetch language JSON data for %s.\n", lang);
                return 1;
            }
            json_error_t error;
            json_t *json = json_loads(lang_json_data, 0, &error);
            free(lang_json_data);
            if (!json) {
                fprintf(stderr, "Error parsing JSON: %s\n", error.text);
                return 1;
            }
            json_t *install_cmd_obj = json_object_get(json, "package_install");
            if (!json_is_string(install_cmd_obj)) {
                fprintf(stderr, "Error: 'install_cmd' is not a string in language JSON.\n");
                json_decref(json);
                return 1;
            }
            install_cmd = strdup(json_string_value(install_cmd_obj));
            if (!install_cmd) {
                fprintf(stderr, "Memory allocation failed for install command.\n");
                json_decref(json);
                return 1;
            }
        }
    }
    else
    {
        lang = get_lang();
        
        install_cmd = get_install();
    }
    char *update_deps_name = malloc(strlen(package_name) + 100);
    if (update_deps_name == NULL) {
        fprintf(stderr, "Memory allocation failed for update_deps_name.\n");
        return 1;
    }
    if(at_sign != NULL)
    {
         snprintf(update_deps_name, strlen(package_name) + 100, "%s@%s", lang ? lang : "default",package_name);
    }
    else
    {
        snprintf(update_deps_name, strlen(package_name) + 100, "%s", package_name);
    }
    if (strcmp(lang, "c") == 0) {
        if (cpkg_main(package_name, lang) != -1) {
            printf("Installed: %s\n", package_name);
            FILE *cf = fopen("project.json", "r");
            if(cf != NULL)
            {
                fclose(cf);
                update_deps(update_deps_name);
            }

            
        }
    } else if (!install_cmd || strcmp(install_cmd, "(null)") == 0) {
        cpkg_main(package_name, lang);
    } else {
        char command[512];
        snprintf(command, sizeof(command), "%s %s", install_cmd, package_name);
        if (system(command) != -1) {
            printf("Installed: %s\n", package_name);
            FILE *cf = fopen("project.json", "r");
            if(cf != NULL)
            {
                fclose(cf);
                update_deps(update_deps_name);
            }
        }
    }

    return 0;
}

int cmd_run(int argc, char **argv) {
    return run_project();
}

int cmd_build(int argc, char **argv) {
    return build_project();
}

int cmd_langs(int argc, char **argv) {
    return show_all_langs();
}
int install_language_support(const char *lang);
int install_lang_support_internal(char *lang)
{
    int res = install_language_support(lang);
    if (res == -1) {
        fprintf(stderr, "Failed to install language support for %s\n", lang);
        return 1;
    }
    return res;
}
int install_lang_support(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s langs <language>\n", argv[0]);
        return 1;
    }
    const char *lang = argv[2];
    int res = install_language_support(lang);
    if (res == -1) {
        fprintf(stderr, "Failed to install language support for %s\n", lang);
        return 1;
    }
    return res;
}
int cmd_ignore(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s ignore <item1> [item2...]\n", argv[0]);
        return 1;
    }

    char **items = malloc((argc - 2) * sizeof(char *));
    if (!items) return -1;

    for (int i = 2; i < argc; ++i)
        items[i - 2] = strdup(argv[i]);

    int res = ignore(items, argc - 2);
    for (int i = 0; i < argc - 2; ++i)
        free(items[i]);
    free(items);
    return res;
}

int cmd_count(int argc, char **argv) {
    return get_line_count();
}

// Command registry
Command commands[] = {
    { "init",     cmd_init,     "Initialize a new project" },
    { "template", cmd_template, "Create a project template" },
    { "install",  cmd_install,  "Install a package" },
    { "run",      cmd_run,      "Run the current project" },
    { "build",    cmd_build,    "Build the current project" },
    { "langs",    cmd_langs,    "List supported languages" },
    { "ignore",   cmd_ignore,   "Ignore files/folders" },
    { "count",    cmd_count,    "Count lines of code" },
    { "install_lang", install_lang_support, "Install language support" }
};

int show_help(const char *prog_name) {
    printf("Usage: %s <command> [args...]\n", prog_name);
    for (size_t i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i)
        printf("  %-10s : %s\n", commands[i].name, commands[i].description);
    printf("  -o           : Enable offline mode (must be last argument)\n");
    return 0;
}

// Entry point
int main(int argc, char **argv) {
    printf("kpm compile date: %s\n", __TIME__);

    if (argc < 2) return show_help(argv[0]);

    // Check offline flag
    if (strcmp(argv[argc - 1], "-o") == 0) {
        offline = true;
        printf("Offline mode enabled (may be buggy).\n");
        argc--; // Don't pass `-o` to handlers
    }

    const char *command = argv[1];

    for (size_t i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
        if (strcmp(commands[i].name, command) == 0)
            return commands[i].handler(argc, argv);
    }

    fprintf(stderr, "Unknown command: %s\n", command);
    return show_help(argv[0]);
}
