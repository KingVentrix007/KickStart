#include "fetch.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parses JSON data and populates the LibraryInfo struct.
 * 
 * This function takes a JSON string as input, parses it using the Jansson library,
 * and populates a LibraryInfo struct with the parsed data. The JSON data is expected
 * to contain information about a library, including its name, git URL, raw path,
 * source paths, header paths, and other metadata.
 * 
 * @param json_data The JSON string to parse.
 * @return LibraryInfo* A pointer to the dynamically allocated LibraryInfo struct
 *                     containing the parsed data. Returns NULL if the JSON parsing
 *                     fails or if memory allocation fails.
 */
LibraryInfo *parse_library_json(const char *json_data) {
  json_error_t error;
  json_t *root = json_loads(json_data, 0, &error);

  if (!root) {
    fprintf(stderr, "Error parsing JSON data: %s\n", error.text);
    return NULL;
  }

  LibraryInfo *lib_info = malloc(sizeof(LibraryInfo));

  // Parse simple fields
  lib_info->name = strdup(json_string_value(json_object_get(root, "name")));
  lib_info->git_url =
      strdup(json_string_value(json_object_get(root, "git_url")));
  lib_info->raw_path =
      strdup(json_string_value(json_object_get(root, "raw_path")));
  lib_info->has_headers =
      json_boolean_value(json_object_get(root, "has_headers"));
  lib_info->is_prebuilt =
      json_boolean_value(json_object_get(root, "is_prebuilt"));
  lib_info->description =
      strdup(json_string_value(json_object_get(root, "description")));
  lib_info->author = strdup(json_string_value(json_object_get(root, "author")));
  lib_info->license =
      strdup(json_string_value(json_object_get(root, "license")));
  lib_info->added_by =
      strdup(json_string_value(json_object_get(root, "added_by")));

  // Parse arrays
  json_t *src_paths_array = json_object_get(root, "src_paths");
  lib_info->src_count = json_array_size(src_paths_array);
  lib_info->src_paths = malloc(lib_info->src_count * sizeof(char *));
  for (size_t i = 0; i < lib_info->src_count; i++) {
    lib_info->src_paths[i] =
        strdup(json_string_value(json_array_get(src_paths_array, i)));
  }

  json_t *header_paths_array = json_object_get(root, "header_paths");
  lib_info->header_count = json_array_size(header_paths_array);
  lib_info->header_paths = malloc(lib_info->header_count * sizeof(char *));
  for (size_t i = 0; i < lib_info->header_count; i++) {
    lib_info->header_paths[i] =
        strdup(json_string_value(json_array_get(header_paths_array, i)));
  }

  json_t *keywords_array = json_object_get(root, "keywords");
  lib_info->keyword_count = json_array_size(keywords_array);
  lib_info->keywords = malloc(lib_info->keyword_count * sizeof(char *));
  for (size_t i = 0; i < lib_info->keyword_count; i++) {
    lib_info->keywords[i] =
        strdup(json_string_value(json_array_get(keywords_array, i)));
  }

  json_decref(root);
  return lib_info;
}

// Function to free the allocated memory for LibraryInfo
void free_library_info(LibraryInfo *lib_info) {
  if (!lib_info)
    return;

  free(lib_info->name);
  free(lib_info->git_url);
  free(lib_info->raw_path);
  free(lib_info->description);
  free(lib_info->author);
  free(lib_info->license);
  free(lib_info->added_by);

  for (size_t i = 0; i < lib_info->src_count; i++) {
    free(lib_info->src_paths[i]);
  }
  free(lib_info->src_paths);

  for (size_t i = 0; i < lib_info->header_count; i++) {
    free(lib_info->header_paths[i]);
  }
  free(lib_info->header_paths);

  for (size_t i = 0; i < lib_info->keyword_count; i++) {
    free(lib_info->keywords[i]);
  }
  free(lib_info->keywords);

  free(lib_info);
}
