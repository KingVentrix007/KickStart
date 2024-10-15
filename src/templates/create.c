#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <jansson.h>  // Include Jansson library for JSON handling
#include "stdbool.h"
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
    printf("Warning. The following code IS deprecated, it is necessary to look at one of the templates, such as c\\rust, and use that to add your new language\n");
    // printf("I do have intentions of updating it, but it is a later project\n");
    return 0;
    char confirm_agreement[MAX_INPUT];
    printf("The following assumptions are made:\n");
    printf("1. You will create ALL the files that will be used,build files,src files,As well as a config.mk\n");
    printf("2. The commands you specify to run ONLY pertain to the template.\n");
    printf("3. Your main.<language_extension> will ONLY contain code to print 'Hello from KickStart' in it.\n");
    printf("4. Your template will be submitted as a pull request to [https://github.com/KingVentrix007/KickStartFiles/tree/main].\n");
    printf("5. You will have acknowledged that your template will be used free of charge by anyone for any means.\n");
    printf("6. Your template does not already exist in the repo.\n");
    get_input("Do you agree to the above assumptions (y/n): ", confirm_agreement, sizeof(confirm_agreement));
    
    if (strcmp(confirm_agreement, "y") != 0) {
        printf("You have not agreed to the above assumptions.\n");
        return 0;
    }
    char lang_name[100];
    int version = 2;
    //Supported systems, mac,linux,windows,etc
    char system_support[100][100];
    size_t num_system_support = 0;

    //If the system supports Libraries;
    bool lib_support;

    //The template paths, not used
    char *version_template_path = NULL;
    
    //Path to the build files
    // char build_file_path[100][100];
    //size_t num_build_file_path = 0;

    //Compiler urls/commands
    char compiler_urls[100][1000];
    size_t num_compiler_urls = 0;

    //Template description
    char description[1000];

    //Template author
    char template_author[100];

    //Git Repo
    char *git_repo = "https://github.com/KingVentrix007/CodeStarterFiles/tree/main/langs/";

    //language license, NOT USED
    char *lang_license = NULL;

    //Name of the default file
    char default_main_file[100];

    //Extensions associated with the code
    char extensions[100][100];
    size_t num_extensions = 0;

    //TODO Make this work
    //Dependencies, NOT really used
    // char dependencies[100][100];

    //Instructions for the project
    char instructions[1000];

    //Template version
    char template_version[100];

    //URL for template update, NOT used
    char *update_url = "N/A";
    
    //Path to git ignore
    char git_ignore_path[sizeof(lang_name)+200];

    //List of folders to create
    char folders_to_create[100][1000];
    size_t num_folders_to_create = 0;

    //List of files to include
    char files_to_include[100][1000];
    size_t num_files_to_include = 0;

    //Commands that must be run
    char commands_to_run[100][1000];
    size_t num_commands_to_run = 0;

    //Path where the main file must be put
    char main_file_path[100];

    //Path in the git repo where the main file template is
    char main_file_template[250];

    //The type if charater that is a comment for that lang
    char comment[100];

    //Commands for the compiler that will be run
    char compiler_cmd[100];
    char package_install[100];
    get_input("Enter language name(E.G c,py): ",lang_name,100);
    get_input("Enter the supported systems(linux,windows,mac). type done when complete: ",system_support[num_system_support],100);
    num_system_support++;
    while(strcmp(system_support[num_system_support-1],"done") != 0)
    {
        get_input("Enter the supported systems(linux,windows,mac). type done when complete: ",system_support[num_system_support],100);
        num_system_support++;
    }

    char lib_sub[100] = {0};
    while (strcmp(lib_sub,"yes") != 0 && strcmp(lib_sub,"no") != 0)
    {
        get_input("Lib support(yes/no): ",lib_sub,100);
    }
    if(strcmp(lib_sub,"yes") == 0)
    {
        lib_support = true;
    }
    else if (strcmp(lib_sub,"no") == 0)
    {
        lib_support = false;
    }

    char build_file_paths[1000][100];
    char build_file_names[1000][100];
    size_t num_build_options_ = 0;
    printf("Please enter the build options in the following prompts, type done to complete\n");
    while(strcmp(build_file_paths[num_build_options_],"done") != 0 && strcmp(build_file_names[num_build_options_],"done") != 0)
    {
        get_input("Enter the build option name(e.g make): ",build_file_names[num_build_options_],sizeof(build_file_names[num_build_options_]));
        get_input("Enter the build path name(e.g c/make): ",build_file_paths[num_build_options_],sizeof(build_file_paths[num_build_options_]));
        if(strcmp(build_file_paths[num_build_options_],"done") == 0 && strcmp(build_file_names[num_build_options_],"done") == 0)
        {
            break;
        }
        num_build_options_++;
        

    }
    get_input("Enter urls/commands for compiler, type done when complete: ",compiler_urls[num_compiler_urls],1000);
    num_compiler_urls++;
    while (strcmp(compiler_urls[num_compiler_urls-1],"done") != 0)
    {
        get_input("Enter urls/commands for compiler, type done when complete: ",compiler_urls[num_compiler_urls],1000);
        num_compiler_urls++;
    }
    get_input("Enter the template description: ",description,1000);
    get_input("Enter the template author: ",template_author,100);
    get_input("Enter the main file(main.c,main.py,main.cpp): ",default_main_file,100);
    
    get_input("Enter the extensions associated with the language(.c,.py,.<extension>). Type done when complete: ",extensions[num_extensions],100);
    num_extensions++;
    while(strcmp(extensions[num_extensions-1],"done") != 0)
    {
        get_input("Enter the extensions associated with the language(.c,.py,.<extension>). Type done when complete: ",extensions[num_extensions],100);
        num_extensions++;
    }
    get_input("Enter instructions for this language: ",instructions,1000);
    
    get_input("Enter folders to create(src,src/project). Type done when complete: ",folders_to_create[num_folders_to_create],1000);
    num_folders_to_create++;
    while(strcmp(folders_to_create[num_folders_to_create -1],"done") != 0)
    {
        get_input("Enter folders to create(src,src/project). Type done when complete: ",folders_to_create[num_folders_to_create],1000);
        num_folders_to_create++;
    }
    get_input("Enter the EXTRA files that must be included. Type done when complete: ",files_to_include[num_files_to_include],1000);
    num_files_to_include++;
    while(strcmp(files_to_include[num_files_to_include -1],"done") != 0)
    {
        get_input("Enter the EXTRA files that must be included: ",files_to_include[num_files_to_include],1000);
        num_files_to_include++;
    }
    get_input("Enter the commands that must be run(go init,npm init). Type done when complete: ",commands_to_run[num_commands_to_run],1000);
    num_commands_to_run++;
    while(strcmp(commands_to_run[num_commands_to_run -1],"done") != 0)
    {
        get_input("Enter the commands that must be run(go init,npm init). Type done when complete: ",commands_to_run[num_commands_to_run],1000);
        num_commands_to_run++;
    }
    get_input("Enter the path where the main file must be placed(src/main.c): ",main_file_path,100);
    printf("Your main file is assumed to be in %s/%s in the KickStartFiles repo\n",lang_name,default_main_file);
    get_input("Enter the characters that comments are(#,//): ",comment,100);
    get_input("Enter the command that must be run to check if the complier is present: ",compiler_cmd,100);
    get_input("Enter command to install a package(pip install,npm install): ",package_install,100);


    json_t *json_obj = json_object();
    json_object_set_new(json_obj, "name", json_string(lang_name));
    json_object_set_new(json_obj, "version", json_integer(version));
    json_t *json_system_support = json_array();

    // Populate the JSON array with values from the system_support array
    for (size_t i = 0; i < num_system_support; i++) {
        json_array_append_new(json_system_support, json_string(system_support[i]));
    }
    json_object_set_new(json_obj, "system_support", json_system_support);
    json_object_set_new(json_obj, "lib_support", json_boolean(lib_support));
    json_object_set_new(json_obj,"version_template_path",json_string(version_template_path));
    json_t *json_build_file_path = json_object();

    // Add key-value pairs to the "build_file_path" object
    // json_object_set_new(json_build_file_path, "makefile", json_string("c/makefile"));
    // json_object_set_new(json_build_file_path, "bash", json_string("c/build.sh"));
    for (size_t i = 0; i < num_build_options_; i++)
    {
        json_object_set_new(json_build_file_path, build_file_names[i], json_string(build_file_paths[i]));
    }
    
    
    // Add the "build_file_path" object to the main JSON object
    json_object_set_new(json_obj, "build_file_path", json_build_file_path);
    json_t *json_compiler_urls = json_array();
     for (size_t i = 0; i < num_compiler_urls; i++) {
        json_array_append_new(json_compiler_urls, json_string(compiler_urls[i]));
    }
    json_object_set_new(json_obj, "compiler_urls", json_compiler_urls);
    json_object_set_new(json_obj,"description",json_string(description));
    json_object_set_new(json_obj,"template_author",json_string(template_author));
    json_object_set_new(json_obj,"git_repo",json_string(git_repo));
    json_object_set_new(json_obj,"lang_license",json_string(lang_license));
    json_object_set_new(json_obj,"default_main_file",json_string(default_main_file));

    json_t *json_extensions = json_array();
    for (size_t i = 0; i < num_extensions; i++)
    {
        json_array_append_new(json_extensions,json_string(extensions[i]));
    }
    json_object_set_new(json_obj,"extensions",json_extensions);
    json_t *json_dependencies = json_array();
    json_object_set_new(json_obj,"dependencies",json_dependencies);
    json_object_set_new(json_obj,"instructions",json_string(instructions));
    json_object_set_new(json_obj,"template_version",json_string(template_version));
    json_object_set_new(json_obj,"update_url",json_string(update_url));
    sprintf(git_ignore_path,"%s/.gitignore",lang_name);
    json_object_set_new(json_obj,"git_ignore_path",json_string(git_ignore_path));

    json_t *json_folders_to_create = json_array();
    for (size_t i = 0; i < num_folders_to_create; i++)
    {
        json_array_append_new(json_folders_to_create,json_string(folders_to_create[i]));
    }
    json_object_set_new(json_obj,"folders_to_create",json_folders_to_create);

    json_t *json_files_to_include = json_array();
    for (size_t i = 0; i < num_files_to_include; i++)
    {
        json_array_append_new(json_files_to_include,json_string(files_to_include[i]));

    }
    json_object_set_new(json_obj,"files_to_include",json_files_to_include);
    
    json_t * json_commands_to_run = json_array();
    for (size_t i = 0; i < num_commands_to_run; i++)
    {
        json_array_append_new(json_commands_to_run,json_string(commands_to_run[i]));
    }
    json_object_set_new(json_obj,"commands_to_run",json_commands_to_run);

    json_object_set_new(json_obj,"main_file_path",json_string(main_file_path));

    sprintf(main_file_template,"%s/%s",lang_name,default_main_file);
    json_object_set_new(json_obj,"default_main_file",json_string(default_main_file));
    json_object_set_new(json_obj,"comment",json_string(comment));
    json_object_set_new(json_obj,"compiler_cmd",json_string(compiler_cmd));
    json_object_set_new(json_obj,"package_install",json_string(package_install));

    

    FILE *file = fopen("lang.json", "w");
    if (!file) {
        perror("Unable to open file");
        return 1;
    }

    // Write the JSON object to the file with pretty formatting
    json_dumpf(json_obj, file, JSON_INDENT(2));

    // Close the file
    fclose(file);

    // Decrement the reference count of the JSON object
    json_decref(json_obj);

    return 0;
}
