# Templates

## What is a Template

A template in KickStart is a combination of a JSON file and supplementary files that define the structure of a project for a specific programming language. The template holds essential information such as the build system, platform, required folders, and other configuration details. For KickStart to support a language, there must be a corresponding template in the [KickStartFiles GitHub repository](https://github.com/KingVentrix007/KickStartFiles/blob/main/langs/).



## Creating a Template
Creating a template is straightforward and can be done in two ways:

### Method 1: Manual Creation
This method involves manually crafting your template by referencing existing ones. You can use other templates as a guide to structure your new template.
### Method 2: Using the Built-in Template Creator
This is the simpler approach and involves using the built-in tool to generate a template.

1. **Run the Template Creator:**

    Use the command:
    ```bash
    kpm template
    ```
    This will initiate the template creation process and prompt you to agree to the terms.

2. **Follow the Prompts:**

    The tool will guide you through the necessary steps to create the template.

3. **Create the Required Files:**

    Once the template creation process is complete, you’ll need to generate the necessary files.

### Key Points
1. **Using Placeholders:**

    Placeholders like ${project_name} can be used in file paths or any other fields where the project name should be dynamically inserted. 
    For example, if your project name is "Example":
    ```bash
    path = src/${project_name}/main.c
    ```
    This will result in:
    ```bash
    src/Example/main.c
    ```
2. **Required Files:**

    Currently, you are required to create the following files, even if they are not used in your project:
    - `Makefile`
    - `config.mk`
    - `build.sh`
    
    You can refer to the C template’s `Makefile` as an example for structure and content.

3. **File Placement:**

    All files included (From the `files_to_include` json field) in the template will be placed at the base directory where KickStart is run. This is something that may be improved in future versions.
    The `files_to_include` field in the template's JSON file lists all the necessary files that will be copied to the project directory during setup. These do NOT include build files and the main file. Look at the python template for an example

## Warnings.
The main file must ONLY contain code to output `Hello from KickStart`. This ensures that templates are clean and uniform. Any additional code will be removed, and you will NOT receive credit for the template.

All commands in the `commands_to_run` json field must ONLY contain commands that are ABSOLUTELY NECESSARY for the project to be setup. If any other commands are used. IT WILL BE MODIFIED, and you will NOT be given any credit.

An exception to the command rule is the `echo` command, which can be used freely

Including any malicious code or commands in a template will result in future contributions not being attributed to the creator. Repeated offenses will lead to contributions being denied entirely, regardless of the nature of the submission.

## Examples
### lang.json
```json
{
    "name": "c",
    "version":2,
    "system_support": [
        "linux"
    ],
    "lib_support": false,
    "version_template_path": "NULL",
    "build_file_path": {
        "makefile": "c/makefile",
        "bash": "c/build.sh"
    },
    "compiler_urls": ["sudo apt-get install gcc"],
    "description": "The c programming languge",
    "template_author": "KingVentrix007",
    "git_repo": "https://github.com/KingVentrix007/CodeStarterFiles/tree/main/langs/",
    "lang_license": {
        "type": "MIT",
        "url": "MIT"
    },
    "default_main_file": "main.c",
    "extensions": [
        ".c"
    ],
    "dependencies": [],
    "instructions": "The instructions field is where you can provide guidance for users setting up the project, such as dependencies or steps required before compiling.",
    "template_version": "1.0.0",
    "update_url": "N/A",
    "git_ignore_path": "c/.gitignore",
    "folders_to_create": [
        "src",
        "include",
        "build",
        "tests",
        "docs",
        "examples",
        "scripts",
        "data",
        "libs"
    ],
    "files_to_include":["config.mk"],
    "commands_to_run": [
        "echo done"
    ],
    "main_file_path": "src/main.c",
    "main_file_template": "c/main.c",
    "comment": "//",
    "compiler_cmd":"gcc -v",
    "package_install":"none" 

}
```
###  Main file
```c
#include <stdio.h>

int main() {
    printf("Hello from KickStart\n");
    return 0;
}
```