#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../licence.h"
#include "config.h"
#include "utils.h"
void create_project_c(
    const char *project_name, const char *project_description, const char *project_author,
    const char *project_license, const char *project_version, const char *project_dependencies,
    const char *generate_readme, const char *initialize_git, const char *create_license_file,
    const char *generate_structure) {

    const char *base_dir = "tests";
    #ifndef DEBUG
    base_dir = ".";
    #endif

    if (strcmp(base_dir, "tests") == 0) {
        system("MKDIR -p tests");
    }

    char main_file_path[1024];
    snprintf(main_file_path, sizeof(main_file_path), "%s/main.c", base_dir);
    FILE *main_file = fopen(main_file_path, "w");
    if (main_file == NULL) {
        perror("Error creating main.c");
        exit(EXIT_FAILURE);
    }

    fprintf(main_file, "// File: main.c\n");
    fprintf(main_file, "// Author: %s\n", project_author);
    fprintf(main_file, "// License: %s\n", project_license);
    fprintf(main_file, "// Version: %s\n", project_version);
    fprintf(main_file, "// Description: %s\n\n", project_description);
    fprintf(main_file, "#include <stdio.h>\n\n");
    fprintf(main_file, "int main() {\n");
    fprintf(main_file, "    printf(\"Hello from KickStart\\n\");\n");
    fprintf(main_file, "    return 0;\n");
    fprintf(main_file, "}\n");
    fclose(main_file);

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
    }

    if (strcmp(create_license_file, "yes") == 0) {
        char license_file_path[1024];
        snprintf(license_file_path, sizeof(license_file_path), "%s/LICENSE", base_dir);
        FILE *license_file = fopen(license_file_path, "w");
        if (license_file == NULL) {
            perror("Error creating LICENSE");
            exit(EXIT_FAILURE);
        }
        char *license_file_content = get_license(project_license);
        fprintf(license_file, "%s", license_file_content);
        free(license_file_content);
        fclose(license_file);
    }

    if (strcmp(generate_structure, "yes") == 0) {
        char MKDIR_cmd[1024];
        snprintf(MKDIR_cmd, sizeof(MKDIR_cmd), "MKDIR -p %s/src %s/build %s/include %s/tests %s/docs %s/examples %s/scripts %s/data %s/libs", 
                base_dir, base_dir, base_dir, base_dir, base_dir, base_dir, base_dir, base_dir, base_dir);
        system(MKDIR_cmd);

        char makefile_path[1024];
        snprintf(makefile_path, sizeof(makefile_path), "%s/Makefile", base_dir);
        FILE *makefile = fopen(makefile_path, "w");
        if (makefile == NULL) {
            perror("Error creating Makefile");
            exit(EXIT_FAILURE);
        }
        fprintf(makefile, "CC = gcc\n");
        fprintf(makefile, "CFLAGS = -Wall -Wextra -Werror -std=c11\n");
        fprintf(makefile, "CONFIG_FILE = ./config.cfg\n\n");
        fprintf(makefile, "include $(CONFIG_FILE)\n\n");

        // Find all .c files in the source directory
        fprintf(makefile, "SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')\n\n");

        // Find all include directories and header files
        fprintf(makefile, "INCLUDE_DIRS = $(shell find $(SRC_DIR) -type d)\n");
        fprintf(makefile, "LIB_INCLUDE_DIRS = $(shell find $(LIBS_DIR) -name 'include' -type d)\n");
        fprintf(makefile, "LIB_INCLUDES = $(foreach dir,$(LIB_INCLUDE_DIRS),-I$(dir))\n");
        fprintf(makefile, "HEADER_FILES = $(shell find $(SRC_DIR) -name '*.h')\n\n");

        // Find all library files
        fprintf(makefile, "LIB_FILES = $(wildcard $(LIBS_DIR)/*/lib/*.a)\n\n");

        // Define the target
        fprintf(makefile, "TARGET = $(BUILD_DIR)/main\n\n");

        // Build rules
        fprintf(makefile, "all: $(TARGET)\n\n");
        fprintf(makefile, "$(TARGET): $(SRC_FILES)\n");
        fprintf(makefile, "\t$(CC) $(CFLAGS) $(LIB_INCLUDES) $(foreach dir,$(INCLUDE_DIRS),-I$(dir)) -o $@ $^ $(LIB_FILES)\n\n");

        // Run the program
        fprintf(makefile, "run: $(TARGET)\n");
        fprintf(makefile, "\t./$(TARGET)\n\n");

        // Clean up
        fprintf(makefile, "clean:\n");
        fprintf(makefile, "\trm -rf $(BUILD_DIR)/*\n\n");

        // Declare phony targets
        fprintf(makefile, ".PHONY: all clean run\n");

        fclose(makefile);

        char config_file_path[1024];
        snprintf(config_file_path, sizeof(config_file_path), "%s/config.cfg", base_dir);
        FILE *config_file = fopen(config_file_path, "w");
        if (config_file == NULL) {
            perror("Error creating config.cfg");
            exit(EXIT_FAILURE);
        }
        fprintf(config_file, "SRC_DIR=./src\n");
        fprintf(config_file, "BUILD_DIR=./build\n");
        fprintf(config_file, "INCLUDE_DIR=./include\n");
        fprintf(config_file, "LIBS_DIR=./libs\n");
        fclose(config_file);
    }

    if (strcmp(initialize_git, "yes") == 0) {
        char gitignore_path[1024];
        snprintf(gitignore_path, sizeof(gitignore_path), "%s/.gitignore", base_dir);
        FILE *gitignore = fopen(gitignore_path, "w");
        if (gitignore == NULL) {
            perror("Error creating .gitignore");
            exit(EXIT_FAILURE);
        }
        fprintf(gitignore, "/build/\n");
        fprintf(gitignore, "*.o\n");
        fprintf(gitignore, "*.a\n");
        fprintf(gitignore, "*.so\n");
        fprintf(gitignore, "*.out\n");
        fprintf(gitignore, "*.exe\n");
        fprintf(gitignore, ".vscode/\n");
        fclose(gitignore);
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

    char project_license_copy[strlen(project_license) + 1];
    strcpy(project_license_copy, project_license);

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
    fprintf(project_json, "    \"language\": \"c\"\n");
    fprintf(project_json, "}\n");
    fclose(project_json);

    char new_main_path[1024];
    snprintf(new_main_path, sizeof(new_main_path), "%s/src/main.c", base_dir);
    rename(main_file_path, new_main_path);
}

