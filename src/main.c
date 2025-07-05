#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "package_manager/cpkg_main.h"
#include "run/run.h"
#include "language.h"

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

    char *lang = get_lang();
    char *install_cmd = get_install();

    if (strcmp(lang, "c") == 0) {
        if (cpkg_main(argv[2], lang) != -1) {
            printf("Installed: %s\n", argv[2]);
            update_deps(argv[2]);
        }
    } else if (!install_cmd || strcmp(install_cmd, "(null)") == 0) {
        cpkg_main(argv[2], lang);
    } else {
        char command[512];
        snprintf(command, sizeof(command), "%s %s", install_cmd, argv[2]);
        if (system(command) != -1) {
            printf("Installed: %s\n", argv[2]);
            update_deps(argv[2]);
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
