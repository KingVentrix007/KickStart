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
    printf("Warning. The following code IS outdated, it is recomended to look at one of the templates, such as py, and use that to add your new language\n");
    char confirm_agreement[MAX_INPUT];
    printf("The following assumptions are made:\n");
    printf("1. You will create a makefile at <language_name>/makefile in your language repo folder.\n");
    printf("2. You will create a bash script at <language_name>/build.sh in your language repo folder.\n");
    printf("3. You will create a .gitignore at <language_name>/.gitignore in your language repo folder.\n");
    printf("4. You will create a main.<lang_extension> at <language_name>/main.<lang_extension> in your language repo folder.\n");
    printf("5. You will create a config.mk at <language_name>/config.mk in your language repo folder.\n");
    printf("6. Your main.<language_extension> will ONLY contain code to print 'Hello from KickStart' in it.\n");
    printf("7. Your template will be submitted as a pull request to https://github.com/KingVentrix007/KickStartFiles/tree/main.\n");
    printf("8. You will have acknowledged that your template will be used free of charge by anyone for any means.\n");
    printf("9. Your template does not already exist in the repo.\n");
    get_input("Do you agree to the above assumptions (y/n): ", confirm_agreement, sizeof(confirm_agreement));
    
    if (strcmp(confirm_agreement, "y") != 0) {
        printf("You have not agreed to the above assumptions.\n");
        return 0;
    }

    char language_name[MAX_INPUT];
    get_input("Enter language name (e.g., cpp): ", language_name, sizeof(language_name));

    char supported_systems[MAX_ENTRIES][MAX_INPUT];
    int supported_systems_count = 0;
    printf("Enter supported systems (e.g., linux, windows, mac). Type 'done' to finish:\n");
    while (supported_systems_count < MAX_ENTRIES) {
        get_input("> ", supported_systems[supported_systems_count], sizeof(supported_systems[supported_systems_count]));
        if (strcmp(supported_systems[supported_systems_count], "done") == 0) {
            break;
        }
        supported_systems_count++;
    }

    char build_types[MAX_ENTRIES][MAX_INPUT];
    int build_types_count = 0;
    printf("Enter build systems (e.g., makefile, bash). Type 'done' to finish:\n");
    while (build_types_count < MAX_ENTRIES) {
        get_input("> ", build_types[build_types_count], sizeof(build_types[build_types_count]));
        if (strcmp(build_types[build_types_count], "done") == 0) {
            break;
        }
        else if (is_supported_build_system(build_types[build_types_count])!=0)
        {
            printf("Supported build types are. makefile,bash: \n");
            printf("Enter build systems (e.g., makefile, bash). Type 'done' to finish:\n");
            // build_types[build_types_count] = '\0';
            memset(build_types[build_types_count],0,MAX_INPUT);

        }
        else
        {
        build_types_count++;

        }
    }

    char lib_support[MAX_INPUT];
    get_input("Enter library support (true/false): ", lib_support, sizeof(lib_support));

    char compiler_urls[MAX_ENTRIES][MAX_INPUT];
    int compiler_urls_count = 0;
    printf("Enter compiler URLs (e.g., https://gcc.gnu.org/install/binaries). Type 'done' to finish:\n");
    while (compiler_urls_count < MAX_ENTRIES) {
        get_input("> ", compiler_urls[compiler_urls_count], sizeof(compiler_urls[compiler_urls_count]));
        if (strcmp(compiler_urls[compiler_urls_count], "done") == 0) {
            break;
        }
        compiler_urls_count++;
    }

    char description[MAX_INPUT];
    get_input("Enter description: ", description, sizeof(description));

    char template_author[MAX_INPUT];
    get_input("Enter template author: ", template_author, sizeof(template_author));

    char lang_license[MAX_INPUT];
    get_input("Enter language license: ", lang_license, sizeof(lang_license));

    char lang_license_url[MAX_INPUT];
    get_input("Enter language license URL: ", lang_license_url, sizeof(lang_license_url));

    char default_main_file[MAX_INPUT];
    get_input("Enter default main file: ", default_main_file, sizeof(default_main_file));

    char extensions[MAX_ENTRIES][MAX_INPUT];
    int extensions_count = 0;
    printf("Enter extensions (e.g., .c, .cpp). Type 'done' to finish:\n");
    while (extensions_count < MAX_ENTRIES) {
        get_input("> ", extensions[extensions_count], sizeof(extensions[extensions_count]));
        if (strcmp(extensions[extensions_count], "done") == 0) {
            break;
        }
        extensions_count++;
    }

    char instructions[MAX_INPUT];
    get_input("Enter instructions: ", instructions, sizeof(instructions));

    char template_version[MAX_INPUT];
    get_input("Enter template version: ", template_version, sizeof(template_version));

    char folders_to_create[MAX_ENTRIES][MAX_INPUT];
    int folders_to_create_count = 0;
    printf("Enter folders to create (e.g., src, include). Type 'done' to finish:\n");
    while (folders_to_create_count < MAX_ENTRIES) {
        get_input("> ", folders_to_create[folders_to_create_count], sizeof(folders_to_create[folders_to_create_count]));
        if (strcmp(folders_to_create[folders_to_create_count], "done") == 0) {
            break;
        }
        folders_to_create_count++;
    }

    char commands_to_run[MAX_ENTRIES][MAX_INPUT];
    int commands_to_run_count = 0;
    printf("Enter commands to run (e.g., make, make install). Type 'done' to finish:\n");
    while (commands_to_run_count < MAX_ENTRIES) {
        get_input("> ", commands_to_run[commands_to_run_count], sizeof(commands_to_run[commands_to_run_count]));
        if (strcmp(commands_to_run[commands_to_run_count], "done") == 0) {
            break;
        }
        commands_to_run_count++;
    }

    char main_file_path[MAX_INPUT];
    get_input("Enter main file path where it will be placed in the user's program(e.g src/cpp): ", main_file_path, sizeof(main_file_path));

    char comment[MAX_INPUT];
    get_input("Enter what prefixes a comment (e.g., //, #): ", comment, sizeof(comment));

    check_files(language_name, build_types, build_types_count, extensions, extensions_count);

    // Create JSON object
    json_t *json_obj = json_object();
    json_object_set_new(json_obj, "name", json_string(language_name));
    json_object_set_new(json_obj, "system_support", json_array());
    for (int i = 0; i < supported_systems_count; i++) {
        json_array_append_new(json_object_get(json_obj, "system_support"), json_string(supported_systems[i]));
    }
    json_object_set_new(json_obj, "build_type", json_array());
    for (int i = 0; i < build_types_count; i++) {
        json_array_append_new(json_object_get(json_obj, "build_type"), json_string(build_types[i]));
    }
    json_object_set_new(json_obj, "lib_support", json_boolean(strcmp(lib_support, "true") == 0));
    json_object_set_new(json_obj, "version_template_path", json_string("NULL")); // Assuming NULL means no specific path

    json_t *build_file_paths = json_object();
    for (int i = 0; i < build_types_count; i++) {
        char path[MAX_INPUT];
        int ret = snprintf(path, sizeof(path), "%s/%s", language_name, build_types[i]);
        if(ret < 0) {
            printf("This should not have happened\n");
            return -1;
        }
        json_object_set_new(build_file_paths, build_types[i], json_string(path));
    }
    json_object_set_new(json_obj, "build_file_path", build_file_paths);

    json_object_set_new(json_obj, "compiler_urls", json_array());
    for (int i = 0; i < compiler_urls_count; i++) {
        json_array_append_new(json_object_get(json_obj, "compiler_urls"), json_string(compiler_urls[i]));
    }

    json_object_set_new(json_obj, "description", json_string(description));
    json_object_set_new(json_obj, "template_author", json_string(template_author));
    json_object_set_new(json_obj, "git_repo", json_string("https://github.com/KingVentrix007/CodeStarterFiles/tree/main/langs/"));
    json_t *lang_license_obj = json_object();
    json_object_set_new(lang_license_obj, "type", json_string(lang_license));
    json_object_set_new(lang_license_obj, "url", json_string(lang_license_url));
    json_object_set_new(json_obj, "lang_license", lang_license_obj);
    json_object_set_new(json_obj, "default_main_file", json_string(default_main_file));
    json_object_set_new(json_obj, "extensions", json_array());
    for (int i = 0; i < extensions_count; i++) {
        json_array_append_new(json_object_get(json_obj, "extensions"), json_string(extensions[i]));
    }
    json_object_set_new(json_obj, "dependencies", json_array()); // Empty array for now
    json_object_set_new(json_obj, "instructions", json_string(instructions));
    json_object_set_new(json_obj, "template_version", json_string(template_version));
    json_object_set_new(json_obj, "update_url", json_string("N/A"));
    json_object_set_new(json_obj, "git_ignore_path", json_string("cpp/.gitignore"));
    json_object_set_new(json_obj, "folders_to_create", json_array());
    for (int i = 0; i < folders_to_create_count; i++) {
        json_array_append_new(json_object_get(json_obj, "folders_to_create"), json_string(folders_to_create[i]));
    }
    json_object_set_new(json_obj, "commands_to_run", json_array());
    for (int i = 0; i < commands_to_run_count; i++) {
        json_array_append_new(json_object_get(json_obj, "commands_to_run"), json_string(commands_to_run[i]));
    }
    json_object_set_new(json_obj, "main_file_path", json_string(main_file_path));
    json_object_set_new(json_obj, "main_file_template", json_string(default_main_file)); // Default main file path
    json_object_set_new(json_obj, "comment", json_string(comment));

    // Write JSON object to file
    if (json_dump_file(json_obj, "lang.json", JSON_INDENT(4)) != 0) {
        perror("Error writing JSON to file");
    }

    // Clean up
    json_decref(json_obj);

    return 1;
}
