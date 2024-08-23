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

#include "licence.h"
#include "ctype.h"

int main_build()
{
     char project_name[1024];
    printf("Enter project name(E.G test_code): ");
    scanf("%s", project_name);
    char project_description[1024*4];
    printf("Enter project description: ");
    getchar();
    fgets(project_description, 1024*4, stdin);
    char project_author[1024];
    printf("Enter project author: ");
    scanf("%s", project_author);
    char project_licence[1024];
    printf("Enter project licence(Type L/l for interactive list): ");
    scanf("%s", project_licence);
    if(strcmp(project_licence, "l") == 0 || strcmp(project_licence, "L") == 0)
    {
        const char *lic = license_menu();
        strcpy(project_licence,lic);
    }
    char project_version[256];
    printf("Enter project version (E.G 1.0.0): ");
    scanf("%s", project_version);
    char project_language[256];
    printf("Enter project programming language (Type L/l for interactive list): ");
    scanf("%s", project_language);
     if(strcmp(project_language, "l") == 0 || strcmp(project_language, "L") == 0)
    {
        const char *lang = language_menu();
        strcpy(project_language,lang);
    }
    char project_dependencies[1024*4];
    printf("Enter project dependencies (comma-separated): ");
    getchar();
    fgets(project_dependencies, 1024*4, stdin);
    char generate_readme[10];
    printf("Generate README file? (yes/no): ");
    scanf("%s", generate_readme);
    char initialize_git[10];
    printf("Initialize Git repository? (yes/no): ");
    scanf("%s", initialize_git);
    char create_license_file[10];
    printf("Create LICENSE file? (yes/no): ");
    scanf("%s", create_license_file);
    char generate_structure[10];
    printf("Generate default file structure? (yes/no): ");
    scanf("%s", generate_structure);
        // printf("")
    lowercase(project_language);
    // Check if the selected language is C
    if (strcmp(project_language, "C") == 0 || strcmp(project_language, "c") == 0) {
        create_project_c(project_name, project_description, project_author, project_licence, project_version, project_dependencies, generate_readme, initialize_git, create_license_file, generate_structure);
    }
    else if (strcmp(project_language, "python") == 0 || strcmp(project_language, "py") == 0) {
        create_project_py(project_name, project_description, project_author, project_licence, project_version, project_dependencies, generate_readme, initialize_git, create_license_file, generate_structure);
    } else {
        printf("Unsupported language: %s\n", project_language);
    }
    return 0;
}