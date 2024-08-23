#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../licence.h"
#include "config.h"
#include "utils.h"



void create_project_py(const char *project_name, const char *project_description, const char *project_author, const char *project_license, const char *project_version, const char *project_dependencies, const char *generate_readme, const char *initialize_git, const char *create_license_file, const char *generate_structure) {
    // Determine base directory based on DEBUG macro
    const char *base_dir = "tests";
    #ifndef DEBUG
    base_dir = ".";
    #endif

    // Ensure base directory exists
    if (strcmp(base_dir, "tests") == 0) {
        system("mkdir -p tests");
    }

    // Create main.py file
    char main_file_path[1024];
    snprintf(main_file_path, sizeof(main_file_path), "%s/main.py", base_dir);
    FILE *main_file = fopen(main_file_path, "w");
    if (main_file == NULL) {
        perror("Error creating main.py");
        exit(EXIT_FAILURE);
    }

    // Write the contents to main.py
    fprintf(main_file, "\"\"\"\n");
    fprintf(main_file, "Project: %s\n", project_name);
    fprintf(main_file, "Author: %s\n", project_author);
    fprintf(main_file, "License: %s\n", project_license);
    fprintf(main_file, "Version: %s\n", project_version);
    fprintf(main_file, "Description: %s\n", project_description);
    fprintf(main_file, "\"\"\"\n\n");
    fprintf(main_file, "def main():\n");
    fprintf(main_file, "    print(\"Hello from CodeStarter\")\n\n");
    fprintf(main_file, "if __name__ == '__main__':\n");
    fprintf(main_file, "    main()\n");
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
        char *license_file_content = get_license(project_license);
        fprintf(license_file, "%s", license_file_content);
        free(license_file_content);
        fclose(license_file);
    }

    // Generate project structure if generate_structure == "yes"
    if (strcmp(generate_structure, "yes") == 0) {
        char mkdir_cmd[1024];
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s/src %s/tests %s/docs %s/examples %s/scripts %s/data", base_dir, base_dir, base_dir, base_dir, base_dir, base_dir);
        system(mkdir_cmd);

        // Create requirements.txt with dependencies
        char requirements_file_path[1024];
        snprintf(requirements_file_path, sizeof(requirements_file_path), "%s/requirements.txt", base_dir);
        FILE *requirements_file = fopen(requirements_file_path, "w");
        if (requirements_file == NULL) {
            perror("Error creating requirements.txt");
            exit(EXIT_FAILURE);
        }
        if (strlen(project_dependencies) > 0) {
            fprintf(requirements_file, "%s\n", project_dependencies);
        }
        fclose(requirements_file);

        // Create .gitignore if git is initialized
        if (strcmp(initialize_git, "yes") == 0) {
            char gitignore_path[1024];
            snprintf(gitignore_path, sizeof(gitignore_path), "%s/.gitignore", base_dir);
            FILE *gitignore = fopen(gitignore_path, "w");
            if (gitignore == NULL) {
                perror("Error creating .gitignore");
                exit(EXIT_FAILURE);
            }
            fprintf(gitignore, "__pycache__/\n");
            fprintf(gitignore, "*.pyc\n");
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

        // Call the function with mutable copies
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
        fprintf(project_json, "    \"dependencies\": \"%s\"\n", project_dependencies_copy);
        fprintf(project_json, "}\n");
        fclose(project_json);
        char new_main_path[1024];
        snprintf(new_main_path, sizeof(new_main_path), "%s/src/main.py", base_dir);
        rename(main_file_path,new_main_path);
    }
}
