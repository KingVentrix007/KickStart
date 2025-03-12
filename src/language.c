#include <stdio.h>
#include "templates/custom.h"
#include <jansson.h>

const char* get_language_name(int choice) {
    switch (choice) {
        case 1: return "C";
        case 2: return "C++";
        case 3: return "Java";
        case 4: return "Go";
        case 5: return "py";
        case 6: return "Rust";
        case 7: return "JavaScript";
        case 8: return "Ruby";
        case 9: return "Swift";
        case 10: return "Kotlin";
        default: return "Invalid choice";
    }
}

const char* language_menu() {
    int choice;

    // Prompt user for input
    printf("These are the officially supported language, there may be more\n" );
    printf("Enter a number between 1 and 10 to select a programming language(Not all are supported):\n");
    printf("1: C\n");
    printf("2: C++\n");
    printf("3: Java(Coming soon)\n");
    printf("4: Go\n");
    printf("5: Python\n");
    printf("6: Rust\n");
    printf("7: JavaScript(Coming soon)\n");
    printf("8: Ruby(Coming soon)\n");
    printf("9: Swift\n");
    printf("10: Kotlin\n");
    scanf("%d", &choice);

    // Get and print the language name based on user choice
    const char* selected_language = get_language_name(choice);
    printf("Selected language: %s\n", selected_language);

    return selected_language;
}
int show_all_langs()
{
    char url[1024];
    snprintf(url, sizeof(url), "%s/index.json", LANG_BASE_URL);

    char *json_string = fetch_json(url);
    // printf("json_data == %s\n",json_data);
    if (!json_string) {
        fprintf(stderr, "Failed to fetch JSON data\n");
        return -1;
    }
    json_error_t error;
    json_t *json = json_loads(json_string, 0, &error);
    if (!json) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return 1;
    }

    // Get the 'langs' object
    json_t *langs = json_object_get(json, "langs");
    if (!langs || !json_is_object(langs)) {
        fprintf(stderr, "'langs' is not an object\n");
        json_decref(json);
        return 1;
    }

    // Iterate through each key in 'langs'
    const char *key;
    json_t *value;
    printf("Languages supported: \n");
    json_object_foreach(langs, key, value) {
        printf("%s\n", key);
    }

    // Clean up
    json_decref(json);

    return 0;
}