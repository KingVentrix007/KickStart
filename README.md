# Kick Start

**Kick Start** is a tool that simplifies the process of starting new coding projects. With just a few steps, users can set up a fully functional project, complete with essential files, templates, and build scripts.

## Features

- **Multi-Language Support**: Kick Start comes with built-in templates for various programming languages, including C, Java, Python, C++, Rust, and more.
- **Build Script Generation**: Automatically generates a Makefile or bash/batch script for supported projects. For Python projects, it creates a `setup.py` file.
- **Library Management**: For languages like C, C++, and Rust, Kick Start creates a `libs` folder and offers a command-line interface to install packages similarly to `pip`. The installed libraries are tracked in a `.json` file and integrated into the Makefile for building.
- **Main File Creation**: A simple main file is generated for each language, containing a default output statement: `"Hello from Kick Start"`.
- **Custom Header File**: Automatically adds a custom header file (or equivalent) with version information and a compile number, which is incremented by the Makefile with each build.
- **Project Structure**: Creates a project directory structure, with separate folders for source code and build files. The project directory includes:
  - A source code folder with the main file.
  - A `build` folder where compiled files are stored.
  - A `.gitignore` file.
  - A `README.md` file.
- **File Header Comments**: Automatically adds a header of comments to the main file, including the author's name, license information, copyright details, and project description.
- **Compiler Detection**: Detects whether the correct compiler is installed on the system. If not, it provides a URL for downloading the appropriate compiler.
- **Custom Templates and Licenses**: Users can create custom templates and licenses. Additional libraries can be added to projects, with each library stored on GitHub in a JSON file. The necessary files are downloaded into the `libs` directory and built into a language-specific library archive (`.a` or the equivalent on Windows).
- **Git Integration**: If Git is installed, Kick Start will initialize a Git repository in the project directory. It will also create an initial commit with the generated files and offer the option to link to a remote repository (GitHub, GitLab, Bitbucket, etc.).
- **Project Configuration Files**:
  - **`project.json`**: This file stores project metadata, including dependencies, project type, license, and other configuration details. The format is compatible with major repository hosting services (e.g., GitHub, GitLab, Bitbucket).
  - **`package.json`**: For languages with package managers, this file includes dependencies and project metadata, similar to Node.js's `package.json`. It can be used by the software to automatically install necessary libraries.

## Future Plans

- **Library Management Enhancements**: The way libraries are managed may evolve, with potential changes to how they are stored, built, and integrated into projects.
- **Custom Build System**: Develop a custom build system similar to `npm` or `npx` that allows for easy management of build scripts, running commands, and integrating dependencies across different languages.
- **Debugger Support**: Include built-in debugger support for each language. For example:
  - **C/C++**: Integration with GDB or LLDB.
  - **Python**: Integration with `pdb` or other Python debuggers.
  - **Rust**: Integration with `gdb` or `rust-lldb`.
- **Common Project Templates**:
  - **Operating System Template**: Creates a basic OS project structure, including a `boot.asm` file and support for bootloaders like GRUB or Limine.
  - **Web Browser/Chat App Templates**: Provides starter templates for common project types like a web browser or chat application, influencing the main file structure and included dependencies.
## Getting Started

## Download
!! Warning !! Downloads may be out of date

To get started with Kick Start, simply run the tool and follow the on-screen prompts to create your project. The tool will guide you through the process of naming your project, providing a description, selecting a license, and more.

### Build
1. Clone the repo
    ```bash
    git clone https://github.com/KingVentrix007/KickStart.git
    ```
2. Enter `KickStart` dir
    ```bash
    cd KickStart
    ```
3. run `make` or `make build
  
    3.1. `make` builds the testing version
    ```bash
    make
    ``` 
    3.2. `make build` builds the release version
    ```bash
    make build
    ``` 
4. Run
    ```bash
    make run # for the testing version, will create a tests dir and build project in there
    ```
    ```bash
    ./kpm <init|template|install> # for release version
    ```
5. Follow the on screen prompts

!! Warning !!
1. The template code is **INCOMPLETE** and will remain so for sometime, please see one of the other lang.json file to learn from
2. ./kpm install only works with C and languages with a built in package manger


---

**Note**: Kick Start is a work in progress, and features may be added, modified, or removed in future updates. Stay tuned for improvements and new functionalities. Please see the [ROADMAP](ROADMAP.md)


