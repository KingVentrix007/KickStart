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
char* get_install() {
    const char *filename = "project.json";
    json_error_t error;
    json_t *root;
    json_t *install;
    char *install_value = NULL;

    // Load and parse the JSON file
    root = json_load_file(filename, 0, &error);
    if(!root) {
        fprintf(stderr, "Error loading JSON file: %s\n", error.text);
        return NULL;
    }

    // Extract the "language" value from the JSON data
    install = json_object_get(root, "install_cmd");
    if(!json_is_string(install)) {
        fprintf(stderr, "Error: install field is missing or not a string\n");
        json_decref(root);
        return NULL;
    }

    // Copy the language value to a new string
    install_value = strdup(json_string_value(install));

    // Clean up
    json_decref(root);

    return install_value;
}
int update_deps(char *installed)
{
    const char *filename = "project.json";
    json_error_t error;
    json_t *root;
    json_t *deps;
    // char *install_value = NULL;

    // Load and parse the JSON file
    root = json_load_file(filename, 0, &error);
    if(!root) {
        fprintf(stderr, "Error loading JSON file: %s\n", error.text);
        return -1;
    }
    deps = json_object_get(root, "dependencies");
    printf("json_is_string(deps) == [%d]\n",json_is_string(deps));
    if(json_is_string(deps) != 0)
    {
        // fprintf(stderr,"Failed to get dependencies\n");
        // return -1;
    }
    char *new_deps = NULL;
    const char *deps_str = json_string_value(deps);
    if(strlen(deps_str) <= 0)
    {
        new_deps = (char *)malloc(strlen(installed)+10);
        memset(new_deps,0,strlen(installed)+10);
    }
    else
    {
        new_deps = (char *)malloc(strlen(deps_str)+strlen(installed)+10);
        memset(new_deps,0,strlen(deps_str)+strlen(installed)+10);

    }
    
    if(new_deps == NULL)
    {
        return -1;
    }
    if(json_is_string(deps) == 1)
    {
        // printf("deps_str == [%s]\n",deps_str);
        // printf("safety = [%d]\n",json_is_string(deps));
        strcat(new_deps,deps_str);
        strcat(new_deps,",");
        if(new_deps[0] == ',')
        {
            new_deps[0] = '\0';
        }
    }
    

    strcat(new_deps,installed);
    // printf("new_deps == [%s]\n",new_deps);
    // new_deps[strlen(deps_str)+strlen(installed)+1] = '\0';
    
    json_t *json_deps = json_string(new_deps);
    if (!json_deps) {
        fprintf(stderr, "Failed to create JSON string from new_deps\n");
        return 1;
    }
    if(json_object_set_new(root,"dependencies",json_deps) != 0)
    {
        fprintf(stderr, "Failed to add JSON string to root\n");
        json_decref(root);
        return 1;
    }
    if((json_dump_file(root, filename, JSON_INDENT(4)) != 0))
    {
        fprintf(stderr, "Failed to write JSON to file\n");
        json_decref(root);
        return 1;
    }

    printf("Successfully updated dependencies\n");
    free(new_deps);
    json_decref(root);
    return 0;
    


}

