#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <jansson.h>  // Include Jansson library for JSON handling

#define MAX_INPUT 1024
#define MAX_ENTRIES 100
const char *supported_build_types[] = {"makefile", "bash", NULL};

int is_supported_build_system(char *system)
{
    for (size_t i = 0; i < 2; i++)
    {
        if(strcmp(system,supported_build_types[i]) == 0)
        {
            return 0;
        }
    }
    return -1;
    
}
// Function to check and create a file if it does not exist
void check_and_create_file(const char *filename, const char *content) {
    if (access(filename, F_OK) != -1) {
        printf("File '%s' already exists.\n", filename);
        return;
    }

    printf("File '%s' is missing. Creating...\n", filename);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
        return;
    }

    if (content != NULL) {
        write(fd, content, strlen(content));
    }

    close(fd);
}

// Function to check and create necessary files based on build systems
void check_files(const char *language_name, char build_system[100][1024], int build_system_count, const char extensions[MAX_ENTRIES][MAX_INPUT], int extensions_count) {
    // Check for main file for each extension
    for (int i = 0; i < extensions_count; i++) {
        char filename[MAX_INPUT];
        snprintf(filename, sizeof(filename), "%s/main%s", language_name, extensions[i]);
        check_and_create_file(filename, "// This is a placeholder main file\n");
    }

    // Check for Makefile and config.mk if 'make' is in the build systems
    int make_found = 0;
    for (int i = 0; i < build_system_count; i++) {
        if (strcmp(build_system[i], "make") == 0) {
            make_found = 1;
            break;
        }
    }

    if (make_found) {
        check_and_create_file("Makefile", "# Makefile placeholder\n");
        check_and_create_file("config.mk", "# config.mk placeholder\n");
    }

    // Check for build scripts based on specified build systems
    for (int i = 0; i < build_system_count; i++) {
        if (strcmp(build_system[i], "bash") == 0) {
            check_and_create_file("build.sh", "#!/bin/bash\n# Build script placeholder\n");
        } else if (strcmp(build_system[i], "batch") == 0) {
            check_and_create_file("build.bat", "@echo off\nREM Batch script placeholder\n");
        } else if (strcmp(build_system[i], "python") == 0) {
            check_and_create_file("build.py", "# Python build script placeholder\n");
        }
    }

    // Check for .gitignore
    check_and_create_file(".gitignore", "# .gitignore placeholder\n");
}

void get_input(char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';  // Remove trailing newline
        }
    }
}

int create_template() {
    printf("Warning. The following code IS deprecated, it is recommended to look at one of the templates, such as py, and use that to add your new language\n");
    printf("I do have intentions of updating it, but it is a later project\n");
    return 0;
    char confirm_agreement[MAX_INPUT];
    printf("The following assumptions are made:\n");
    printf("1. You will create a makefile at <language_name>/makefile in your language repo folder.\n");
    printf("2. You will create a bash script at <language_name>/build.sh in your language repo folder.\n");
    printf("3. You will create a .gitignore at <language_name>/.gitignore in your language repo folder.\n");
    printf("4. You will create a main.<lang_extension> at <language_name>/main.<lang_extension> in your language repo folder.\n");
    printf("5. You will create a config.mk at <language_name>/config.mk in your language repo folder.\n");
    printf("6. Your main.<language_extension> will ONLY contain code to print 'Hello from KickStart' in it.\n");
    printf("7. Your template will be submitted as a pull request to [https://github.com/KingVentrix007/KickStartFiles/tree/main].\n");
    printf("8. You will have acknowledged that your template will be used free of charge by anyone for any means.\n");
    printf("9. Your template does not already exist in the repo.\n");
    get_input("Do you agree to the above assumptions (y/n): ", confirm_agreement, sizeof(confirm_agreement));
    
    if (strcmp(confirm_agreement, "y") != 0) {
        printf("You have not agreed to the above assumptions.\n");
        return 0;
    }

    char lang_name[1024];
    get_input("Enter the name of the language: ",lang_name,1000);
    char system_support[MAX_ENTRIES][MAX_INPUT];
    char curr_system[MAX_INPUT];
    size_t curr_system_pos = 0;
    while(strcmp(curr_system,"done") !=0)
    {
        get_input("Enter system support(e.g linux) type 'done' to finish: ",curr_system,MAX_INPUT);
        strcpy(system_support[curr_system_pos],curr_system);
        curr_system_pos++;

    }
    memset(system_support[--curr_system_pos],0,MAX_INPUT);
    char lib_support[10];
    get_input("Builtin lib support true/false" ,lib_support,10);
    //TODO 
    // char *version_template_path =  "NULL";
    char build_file_name[1024][1024];
    char build_file_paths[1024][1024];
    // build_file_path = 
    // {    
    //      build_file_name[x]:build_file_paths[x]
    // 
    // }
    size_t build_file_count = 0;
    for (size_t i = 0; i < 1024; i++)
    {
        char build_file_name_cur[1024];
        char build_file_paths_cur[1024];
        get_input("Enter a build system(make,python,bash) or done to finish: ",build_file_name_cur,1024);
        if(strcmp(build_file_name_cur,"done") == 0)
        {
            break;
        }
        get_input("Enter the path corresponding to your build system(c/make,c/python,c/bash): ",build_file_paths_cur,1024);
        strcpy(build_file_name[build_file_count],build_file_name_cur);
        strcpy(build_file_paths[build_file_count],build_file_paths_cur);
        build_file_count++;

    }
    char compiler_urls[1024][1024];
    size_t compiler_urls_count = 0;
    for (size_t i = 0; i < 1024; i++)
    {
       char compiler_url_cur[1024];
       get_input("Enter a complier url or command for your language or 'done' to finish: ",compiler_url_cur,1024);
       if(strcmp(compiler_url_cur,"done") == 0)
       {
        break;
       }
       strcpy(compiler_urls[compiler_urls_count],compiler_url_cur);
       compiler_urls_count++;
       
    }
    char description[1024];
    get_input("Please enter a description of your language: ",description,1024);
    char template_author[1024];
    get_input("Please enter the template author: ",template_author,1024);
    //TODO 
    // char *git_repo = "https://github.com/KingVentrix007/CodeStarterFiles/tree/main/langs/";
    
    return 1;
}
