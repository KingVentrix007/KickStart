#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
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