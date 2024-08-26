#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./templates/c.h"
#include "./templates/py.h"
#include "./templates/utils.h"
#include "language.h"
#include "./templates/config.h"
#include "licence.h"
#include "ctype.h"
int create_project(char *project_name, char *project_description, char *project_author,char *project_licence, char *project_version, char *project_language,char *project_dependencies, char *generate_readme, char *initialize_git,char *create_license_file);
int main_build() {
    char project_name[1024] = "my_project";
    printf("Enter project name (e.g., test_code) [default: %s]: ", project_name);
    fgets(project_name, sizeof(project_name), stdin);
    if (project_name[0] == '\n') {
        strcpy(project_name, "my_project");
    } else {
        project_name[strcspn(project_name, "\n")] = 0;  // Remove trailing newline
    }
    
    char project_description[4096] = "";
    printf("Enter project description (optional): ");
    fgets(project_description, sizeof(project_description), stdin);
    project_description[strcspn(project_description, "\n")] = 0;  // Remove trailing newline

    char project_author[1024] = "anonymous";
    printf("Enter author name [default: %s]: ", project_author);
    fgets(project_author, sizeof(project_author), stdin);
    if (project_author[0] == '\n') {
        strcpy(project_author, "anonymous");
    } else {
        project_author[strcspn(project_author, "\n")] = 0;  // Remove trailing newline
    }

    char project_license[1024] = "MIT";
    printf("Enter license (type L/l for a list) [default: %s]: ", project_license);
    fgets(project_license, sizeof(project_license), stdin);
    if (project_license[0] == '\n') {
        strcpy(project_license, "MIT");
    } else {
        project_license[strcspn(project_license, "\n")] = 0;  // Remove trailing newline
        if (strcmp(project_license, "l") == 0 || strcmp(project_license, "L") == 0) {
            const char *lic = license_menu();
            strcpy(project_license, lic);
        }
    }

    char project_version[256] = "1.0.0";
    printf("Enter project version (e.g., 1.0.0) [default: %s]: ", project_version);
    fgets(project_version, sizeof(project_version), stdin);
    if (project_version[0] == '\n') {
        strcpy(project_version, "1.0.0");
    } else {
        project_version[strcspn(project_version, "\n")] = 0;  // Remove trailing newline
    }

    char project_language[256] = "C";
    printf("Enter programming language (type L/l for a list) [default: %s]: ", project_language);
    fgets(project_language, sizeof(project_language), stdin);
    if (project_language[0] == '\n') {
        strcpy(project_language, "C");
    } else {
        project_language[strcspn(project_language, "\n")] = 0;  // Remove trailing newline
        if (strcmp(project_language, "l") == 0 || strcmp(project_language, "L") == 0) {
            const char *lang = language_menu();
            strcpy(project_language, lang);
        }
    }

    char project_dependencies[4096] = "";
    printf("Enter project dependencies (comma-separated, optional): ");
    fgets(project_dependencies, sizeof(project_dependencies), stdin);
    project_dependencies[strcspn(project_dependencies, "\n")] = 0;  // Remove trailing newline

    char generate_readme[10] = "yes";
    printf("Generate README file? (yes/no) [default: %s]: ", generate_readme);
    fgets(generate_readme, sizeof(generate_readme), stdin);
    if (generate_readme[0] == '\n') {
        strcpy(generate_readme, "yes");
    } else {
        generate_readme[strcspn(generate_readme, "\n")] = 0;  // Remove trailing newline
    }

    char initialize_git[10] = "yes";
    printf("Initialize Git repository? (yes/no) [default: %s]: ", initialize_git);
    fgets(initialize_git, sizeof(initialize_git), stdin);
    if (initialize_git[0] == '\n') {
        strcpy(initialize_git, "yes");
    } else {
        initialize_git[strcspn(initialize_git, "\n")] = 0;  // Remove trailing newline
    }

    char create_license_file[10] = "yes";
    printf("Create LICENSE file? (yes/no) [default: %s]: ", create_license_file);
    fgets(create_license_file, sizeof(create_license_file), stdin);
    if (create_license_file[0] == '\n') {
        strcpy(create_license_file, "yes");
    } else {
        create_license_file[strcspn(create_license_file, "\n")] = 0;  // Remove trailing newline
    }

// #ifndef DEBUG
//     char generate_structure[10] = "yes";
//     printf("Generate default file structure? (yes/no) [default: yes]: ");
//     fgets(generate_structure, sizeof(generate_structure), stdin);
//     generate_structure[strcspn(generate_structure, "\n")] = 0;  // Remove trailing newline
// #else
    char *generate_structure = "yes";
// #endif

    lowercase(project_language);

    if (strcmp(project_language, "c") == 0) {
        create_project_c(project_name, project_description, project_author, project_license, project_version, project_dependencies, generate_readme, initialize_git, create_license_file, generate_structure);
    } else if (strcmp(project_language, "python") == 0 || strcmp(project_language, "py") == 0) {
        create_project_py(project_name, project_description, project_author, project_license, project_version, project_dependencies, generate_readme, initialize_git, create_license_file, generate_structure);
    } else {
        printf("Searching for language template %s\n", project_language);

        int ret = create_project(project_name, project_description, project_author, project_license, project_version, project_language, project_dependencies, generate_readme, initialize_git, create_license_file);
        if(ret != 0)
        {
            printf("Failed to build project %s for language %s\n",project_name,project_language);
            printf("This can be caused by\n\t-No internet connection - The templates ar stored on github repo, and require an internet connection\n");
            printf("\t-The language is not suported. In this case, please head to https://github.com/KingVentrix007/KickStartFiles/tree/main and add a template for your language\n");
        }
    }

    return 0;
}