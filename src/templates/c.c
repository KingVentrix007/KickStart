#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEBUG
void remove_trailing_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
void create_project_c(const char *project_name, const char *project_description, const char *project_author, const char *project_license, const char *project_version, const char *project_dependencies, const char *generate_readme, const char *initialize_git, const char *create_license_file, const char *generate_structure) {
    // Determine base directory based on DEBUG macro
    const char *base_dir = "tests";
    #ifndef DEBUG
    base_dir = ".";
    #endif

    // Ensure base directory exists
    if (strcmp(base_dir, "tests") == 0) {
        system("mkdir -p tests");
    }

    // Create main.c file
    char main_file_path[1024];
    snprintf(main_file_path, sizeof(main_file_path), "%s/main.c", base_dir);
    FILE *main_file = fopen(main_file_path, "w");
    if (main_file == NULL) {
        perror("Error creating main.c");
        exit(EXIT_FAILURE);
    }

    // Write the contents to main.c
    fprintf(main_file, "// File: main.c\n");
    fprintf(main_file, "// Author: %s\n", project_author);
    fprintf(main_file, "// License: %s\n", project_license);
    fprintf(main_file, "// Version: %s\n", project_version);
    fprintf(main_file, "// Description: %s\n\n", project_description);
    fprintf(main_file, "#include <stdio.h>\n\n");
    fprintf(main_file, "int main() {\n");
    fprintf(main_file, "    printf(\"Hello from CodeStarter\\n\");\n");
    fprintf(main_file, "    return 0;\n");
    fprintf(main_file, "}\n");
    fclose(main_file);

    // Create README.md if generate_readme == "yes"
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
        fprintf(readme_file, "# Installation\n\n");
        fprintf(readme_file, "# Usage\n\n");
        fprintf(readme_file, "# License\n\n");
        fprintf(readme_file, "# Credits\n\n");
        fprintf(readme_file, "# Contact\n\n");
        fprintf(readme_file, "# Acknowledgments\n\n");
        fclose(readme_file);
    }

    // Initialize Git if initialize_git == "yes"
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

    // Create LICENSE file if create_license_file == "yes"
    if (strcmp(create_license_file, "yes") == 0) {
        char license_file_path[1024];
        snprintf(license_file_path, sizeof(license_file_path), "%s/LICENSE", base_dir);
        FILE *license_file = fopen(license_file_path, "w");
        if (license_file == NULL) {
            perror("Error creating LICENSE");
            exit(EXIT_FAILURE);
        }
        fprintf(license_file, "%s License\n", project_license);
        fclose(license_file);
    }

    // Generate project structure if generate_structure == "yes"
    if (strcmp(generate_structure, "yes") == 0) {
        char mkdir_cmd[1024];
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s/src %s/build %s/include %s/tests %s/docs %s/examples %s/scripts %s/data", base_dir, base_dir, base_dir, base_dir, base_dir, base_dir, base_dir, base_dir);
        system(mkdir_cmd);

        // Create Makefile
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
        fprintf(makefile, "SRC_FILES = $(wildcard $(SRC_DIR)/*.c)\n");
        fprintf(makefile, "TARGET = $(BUILD_DIR)/main\n\n");
        fprintf(makefile, "all: $(TARGET)\n\n");
        fprintf(makefile, "$(TARGET): $(SRC_FILES)\n");
        fprintf(makefile, "\t$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $^\n\n");
        fprintf(makefile, "run: $(TARGET)\n");
        fprintf(makefile, "\t./$(TARGET)\n\n");
        fprintf(makefile, "clean:\n");
        fprintf(makefile, "\trm -rf $(BUILD_DIR)/*\n\n");
        fprintf(makefile, ".PHONY: all clean run\n");
        fclose(makefile);

        // Create config file (config.cfg)
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
        fclose(config_file);
    }

    // Create .gitignore if git is initialized
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

    // Create project.json
    char project_json_path[1024];
    snprintf(project_json_path, sizeof(project_json_path), "%s/project.json", base_dir);
    FILE *project_json = fopen(project_json_path, "w");
    if (project_json == NULL) {
        perror("Error creating project.json");
        exit(EXIT_FAILURE);
    }
    remove_trailing_newline(project_name);
remove_trailing_newline(project_version);
remove_trailing_newline(project_description);
remove_trailing_newline(project_author);
remove_trailing_newline(project_license);
remove_trailing_newline(project_dependencies);

fprintf(project_json, "{\n");
fprintf(project_json, "    \"name\": \"%s\",\n", project_name);
fprintf(project_json, "    \"version\": \"%s\",\n", project_version);
fprintf(project_json, "    \"description\": \"%s\",\n", project_description);
fprintf(project_json, "    \"author\": \"%s\",\n", project_author);
fprintf(project_json, "    \"license\": \"%s\",\n", project_license);
fprintf(project_json, "    \"dependencies\": \"%s\"\n", project_dependencies);
fprintf(project_json, "}\n");
fclose(project_json);
char new_main_path[1024];
 snprintf(new_main_path, sizeof(new_main_path), "%s/src/main.c", base_dir);
rename(main_file_path,new_main_path);
}
