#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Placeholder function to check JSON file for package information
int check_json_for_package(const char *package) {
    printf("Cannot find %s\n",package);
    // TODO: Implement JSON file checking logic
    return 0;
}

// Function to build a package
int build_package(const char *package, const char *temp_dir) {
    char command[512];
    
    if (strcmp(package, "libcurl") == 0) {
        snprintf(command, sizeof(command), "cd %s && ./configure --disable-shared", temp_dir);
        if (system(command) != 0) {
            return 1;
        }

        snprintf(command, sizeof(command), "cd %s && make", temp_dir);
        if (system(command) != 0) {
            return 1;
        }

        snprintf(command, sizeof(command), "cd %s && make install", temp_dir);
        if (system(command) != 0) {
            return 1;
        }
    } else {
        // Add support for other packages if needed
    }
    
    return 0;
}

void copy_files(const char *package, const char *temp_dir) {
    char command[512];
    
    // Ensure directory paths are correctly specified
    snprintf(command, sizeof(command), "mkdir -p libs/%s/include && cp %s/include/*.h libs/%s/include/",
             package, temp_dir, package);
    system(command);

    snprintf(command, sizeof(command), "mkdir -p libs/%s/lib && cp %s/lib/*.a libs/%s/lib/",
             package, temp_dir, package);
    system(command);
}
int install_package_c(char *package) {
    // List of packages and their Git URLs
    struct {
        const char *name;
        const char *git_url;
    } packages[] = {
        {"libcurl", "https://github.com/curl/curl.git"},
        {"sqlite", "https://github.com/sqlite/sqlite.git"},
        {"zlib", "https://github.com/madler/zlib.git"},
        {"openssl", "https://github.com/openssl/openssl.git"},
        {"libpng", "https://github.com/glennrp/libpng.git"},
        {"sdl2", "https://github.com/libsdl-org/SDL.git"},
        {"boost", "https://github.com/boostorg/boost.git"},
        {"ncurses", "https://github.com/mirror/ncurses.git"},
        {"libxml2", "https://gitlab.gnome.org/GNOME/libxml2.git"},
        {"jansson", "https://github.com/akheron/jansson.git"}
    };

    // Search for the package in the predefined list
    const char *git_url = NULL;
    for (size_t i = 0; i < sizeof(packages) / sizeof(packages[0]); ++i) {
        if (strcmp(package, packages[i].name) == 0) {
            git_url = packages[i].git_url;
            break;
        }
    }

    // If not found in the predefined list, check the JSON file
    if (git_url == NULL) {
        if (!check_json_for_package(package)) {
            printf("Cannot find package with name %s\n", package);
            return 1;
        }
        // Placeholder: set git_url from JSON file result
        git_url = "https://example.com/fallback.git";
    }

    // Create a temporary directory for cloning
    char temp_dir[256];
    snprintf(temp_dir, sizeof(temp_dir), "/tmp/%s", package);

    // Ensure the command length is safe
    char command[512];  // Increased buffer size
    snprintf(command, sizeof(command), "git clone %s %s", git_url, temp_dir);
    if (system(command) != 0) {
        printf("Failed to clone repository for package %s\n", package);
        return 1;
    }

    // Build the package
    if (build_package(package, temp_dir) != 0) {
        printf("Failed to build package %s\n", package);
        return 1;
    }

    // Copy header and library files
    copy_files(package, temp_dir);

    // Clean up
    snprintf(command, sizeof(command), "rm -rf %s", temp_dir);
    if (system(command) != 0) {
        printf("Failed to remove temporary directory %s\n", temp_dir);
        return 1;
    }

    printf("Package %s installed successfully\n", package);
    return 0;
}
