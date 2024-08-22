#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>






int main(void)
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
    printf("Enter project licence(E.G MIT,GPL,BSD): ");
    scanf("%s", project_licence);
    char project_version[256];
    printf("Enter project version (E.G 1.0.0): ");
    scanf("%s", project_version);
    char project_language[256];
    printf("Enter project programming language (E.G C, Python, Java): ");
    scanf("%s", project_language);
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
    
    return 0;
}
