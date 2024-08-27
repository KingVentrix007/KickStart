#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include "package_manager/cpkg_main.h"
        int create_template();


char* get_lang();
char* get_lang() {
    const char *filename = "project.json";
    json_error_t error;
    json_t *root;
    json_t *language;
    char *lang_value = NULL;

    // Load and parse the JSON file
    root = json_load_file(filename, 0, &error);
    if(!root) {
        fprintf(stderr, "Error loading JSON file: %s\n", error.text);
        return NULL;
    }

    // Extract the "language" value from the JSON data
    language = json_object_get(root, "language");
    if(!json_is_string(language)) {
        fprintf(stderr, "Error: Language field is missing or not a string\n");
        json_decref(root);
        return NULL;
    }

    // Copy the language value to a new string
    lang_value = strdup(json_string_value(language));

    // Clean up
    json_decref(root);

    return lang_value;
}
int main_build();
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <init|install> [package_name]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        main_build();
    } else if (strcmp(argv[1], "install") == 0) {
        // printf("Package manager is not enabled at the moment\n");
        // return 0;
        if (argc < 3) {
            fprintf(stderr, "Usage: %s install <package_name>\n", argv[0]);
            return 1;
        }

        char *lang = get_lang();
        if (strcmp(lang, "c") == 0) {
            cpkg_main(argv[2], lang);
        } else {
            fprintf(stderr, "Unsupported language: %s\n", lang);
            return 1;
        }
    }else if (strcmp(argv[1],"template") == 0)
    {
        create_template();
    }
     
    else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}