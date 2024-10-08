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

    Use `${project_name}` as a placeholder wherever the project name needs to be inserted. For example, if your project name is "Example":
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

    All files included in the template will be placed at the base directory where KickStart is run. This is something that may be improved in future versions.

### Known Bug
Sometimes, files or folders named done may be created. This is a known bug and you can safely delete these files.