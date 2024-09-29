#include "errno.h"
#include "fetch.h"
#include "jansson.h"
#include <curl/curl.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define INDEX_URL                                                              \
  "https://raw.githubusercontent.com/KingVentrix007/CodeStarterFiles/main/"    \
  "libs"

#define INDEX_NAME "index.json"

/**
 * @brief A structure to store the response data from HTTP requests
 * 
 */
struct MemoryStruct {
  char *memory;
  size_t size;
};

/**
 * @brief Callback function to handle data received from cURL.
 * 
 * This function is called by cURL to write the received data into a `MemoryStruct`.
 * It dynamically resizes the memory buffer to accommodate the incoming data and
 * appends the data to the buffer.
 * 
 * @param contents Pointer to the data received from cURL.
 * @param size Size of each element in the data.
 * @param nmemb Number of elements in the data.
 * @param userp Pointer to the user data, which in this case is a `MemoryStruct`.
 * @return size_t The number of bytes successfully written.
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == NULL) {
    // Out of memory
    printf("Not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

/**
 * @brief Fetches the index.json file from the specified URL and returns its content as a string.
 * 
 * This function initializes a cURL session, sets up the necessary options to fetch the index.json file,
 * performs the HTTP request, and stores the response in a dynamically allocated memory buffer.
 * 
 * @return char* A pointer to the dynamically allocated memory buffer containing the content of index.json.
 *               Returns NULL if the fetch operation fails.
 */
char *fetch_index_json() {
  CURL *curl_handle;
  CURLcode res;
  struct MemoryStruct chunk;

  chunk.memory = malloc(1); // will be grown as needed by the realloc above
  chunk.size = 0;           // no data at this point

  curl_global_init(CURL_GLOBAL_ALL);

  curl_handle = curl_easy_init();
  char *url = malloc(strlen(INDEX_URL) + strlen("index.json") + 100);
  int ret = snprintf(url, strlen(INDEX_URL) + strlen("index.json") + 100,
                     "%s/%s", INDEX_URL, INDEX_NAME);
  if (ret) {
  }
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  res = curl_easy_perform(curl_handle);

  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    free(chunk.memory);
    chunk.memory = NULL;
  }

  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return chunk.memory;
}

/**
 * @brief Fetches JSON data from a given URL and returns its content as a string.
 * 
 * This function initializes a cURL session, sets up the necessary options to fetch JSON data from the specified URL,
 * performs the HTTP request, and stores the response in a dynamically allocated memory buffer.
 * 
 * @param url The URL from which to fetch the JSON data.
 * @return char* A pointer to the dynamically allocated memory buffer containing the content of the JSON data.
 *               Returns NULL if the fetch operation fails.
 */
char *fetch_json_data(const char *url) {
  CURL *curl_handle;
  CURLcode res;
  struct MemoryStruct chunk;

  chunk.memory = malloc(1);
  chunk.size = 0;

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();

  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  res = curl_easy_perform(curl_handle);

  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    free(chunk.memory);
    chunk.memory = NULL;
  }

  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return chunk.memory;
}

/**
 * @brief Retrieves the path for a specific library name and language from the index.json file.
 * 
 * This function fetches the index.json file, parses it, and searches for the specified library name and language.
 * If the library is found and the language matches, it returns the path associated with the library.
 * 
 * @param lib_name The name of the library to search for.
 * @param language The language of the library to match.
 * @return char* A pointer to a dynamically allocated string containing the path of the library.
 *               Returns NULL if the library is not found, the language does not match, or an error occurs.
 */
char *get_lib_path(const char *lib_name, const char *language) {
  char *json_data = fetch_index_json();
  if (!json_data) {
    printf("Failed to fetch index.json\n");
    return NULL;
  }

  json_error_t error;
  json_t *root = json_loads(json_data, 0, &error);
  free(json_data);

  if (!root) {
    fprintf(stderr, "Error parsing JSON data: %s\n", error.text);
    return NULL;
  }

  json_t *lib_info = json_object_get(root, lib_name);
  if (!lib_info) {
    printf("Library %s not found in index.json\n", lib_name);
    json_decref(root);
    return NULL;
  }

  const char *lib_lang = json_string_value(json_object_get(lib_info, "lang"));
  if (!lib_lang || strcmp(lib_lang, language) != 0) {
    printf("Language mismatch for library %s. Expected: %s, Found: %s\n",
           lib_name, language, lib_lang ? lib_lang : "N/A");
    json_decref(root);
    return NULL;
  }

  const char *path = json_string_value(json_object_get(lib_info, "path"));
  if (!path) {
    printf("Path not found for library %s\n", lib_name);
    json_decref(root);
    return NULL;
  }

  char *result = strdup(path);
  json_decref(root);
  return result;
}

/**
 * @brief Fetches a file from a specified URL and saves it to a local path.
 * 
 * This function initializes a cURL session, sets up the necessary options to fetch a file from the specified URL,
 * performs the HTTP request, and saves the response to the specified local path.
 * 
 * @param url The URL from which to fetch the file.
 * @param local_path The local path where the fetched file should be saved.
 * @return int Returns 0 if the file was successfully fetched and saved. Returns -1 if an error occurs.
 */
int fetch_and_save_file(const char *url, const char *local_path) {
  CURL *curl_handle;
  FILE *fp;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl_handle = curl_easy_init();

  if (curl_handle) {
    fp = fopen(local_path, "wb");
    if (fp == NULL) {
      curl_easy_cleanup(curl_handle);
      return -1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);

    res = curl_easy_perform(curl_handle);
    fclose(fp);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      return -1;
    }

    curl_easy_cleanup(curl_handle);
  }

  curl_global_cleanup();
  return 0;
}

/**
 * @brief Create a directory if it doesn't exist
 * 
 * @param path 
 */
void create_dir(const char *path) {
  struct stat st = {0};
  if (stat(path, &st) == -1) {
    mkdir(path, 0700);
  }
}

/**
 * @brief Creates directories recursively if they don't exist.
 * 
 * This function takes a path and ensures that all directories in the path are created.
 * It handles the creation of intermediate directories if they do not exist.
 * 
 * @param path The path for which directories need to be created.
 */
void create_dirs_recursively(const char *path) {
  char tmp[512];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp), "%s", path);
  len = strlen(tmp);

  // Remove the filename from the path, keeping only the directory part
  if (tmp[len - 1] != '/') {
    char *dir = dirname(tmp);
    snprintf(tmp, sizeof(tmp), "%s", dir);
  }

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, 0700);
      *p = '/';
    }
  }
  mkdir(tmp, 0700);
}

/**
 * @brief Saves header files from the library information to the specified directory.
 * 
 * This function iterates over the list of header file paths in the library information,
 * constructs the local path for each header file, ensures the necessary directories exist,
 * constructs the URL for each header file, and fetches and saves the header files to the local paths.
 * 
 * @param lib_info Pointer to the LibraryInfo structure containing the library information.
 */
void save_header_files(LibraryInfo *lib_info) {
  char base_dir[256];
  snprintf(base_dir, sizeof(base_dir), "libs/%s", lib_info->name);

  for (size_t i = 0; i < lib_info->header_count; i++) {
    char local_path[512];
    snprintf(local_path, sizeof(local_path), "%s/%s", base_dir,
             lib_info->header_paths[i]);

    // Extract the directory path from local_path
    char local_dir[512];
    snprintf(local_dir, sizeof(local_dir), "%s", local_path);
    char *dir_name = dirname(local_dir);

    // Create necessary directories for the file path, excluding the file itself
    create_dirs_recursively(dir_name);

    char file_url[512];
    snprintf(file_url, sizeof(file_url), "%s%s", lib_info->raw_path,
             lib_info->header_paths[i]);

    printf("Fetching header file from %s\n", file_url);
    printf("Saving header file to %s\n", local_path);

    if (fetch_and_save_file(file_url, local_path) == 0) {
      printf("Header file saved successfully.\n");
    } else {
      printf("Failed to save header file.\n");
    }
  }
}

/**
 * @brief Saves source files from the library information to the specified directory.
 * 
 * This function iterates over the list of source file paths in the library information,
 * constructs the local path for each source file, ensures the necessary directories exist,
 * constructs the URL for each source file, and fetches and saves the source files to the local paths.
 * 
 * @param lib_info Pointer to the LibraryInfo structure containing the library information.
 */
void save_source_files(LibraryInfo *lib_info) {
  char base_dir[256];
  snprintf(base_dir, sizeof(base_dir), "libs/%s", lib_info->name);

  for (size_t i = 0; i < lib_info->src_count; i++) {
    char local_path[512];
    snprintf(local_path, sizeof(local_path), "%s/%s", base_dir,
             lib_info->src_paths[i]);

    // Extract the directory path from local_path
    char local_dir[512];
    snprintf(local_dir, sizeof(local_dir), "%s", local_path);
    char *dir_name = dirname(local_dir);

    // Create necessary directories for the file path, excluding the file itself
    create_dirs_recursively(dir_name);

    char file_url[512];
    snprintf(file_url, sizeof(file_url), "%s%s", lib_info->raw_path,
             lib_info->src_paths[i]);

    printf("Fetching file from %s\n", file_url);
    printf("Saving file to %s\n", local_path);

    if (fetch_and_save_file(file_url, local_path) == 0) {
      printf("File saved successfully.\n");
    } else {
      printf("Failed to save file.\n");
    }
  }
}

/**
 * @brief Checks if a directory exists at the specified path.
 * 
 * This function attempts to open the directory at the given path. If the directory
 * exists, it returns 1. If the directory does not exist, it returns 0. If an error
 * occurs while trying to open the directory, it returns -1 and prints an error message.
 * 
 * @param path The path of the directory to check.
 * @return int Returns 1 if the directory exists, 0 if it does not exist, and -1 if an error occurs.
 */
int directory_exists(const char *path) {
  DIR *dir = opendir(path);
  if (dir) {
    closedir(dir);
    return 1; // Directory exists
  } else if (ENOENT == errno) {
    return 0; // Directory does not exist
  } else {
    perror("opendir");
    return -1; // Error occurred
  }
}

// int main() {
//     const char *lib_name = "buffer";  // replace with the actual library name
//     you're looking for const char *language = "c";       // replace with the
//     actual language you're looking for char *path = get_lib_path(lib_name,
//     language); if(directory_exists("libs") != 1)
//     {
//         mkdir("libs",0700);
//     }
//     char *lib_dir_path = malloc(strlen(lib_name)+strlen("libs")+20);
//     snprintf(lib_dir_path, strlen(lib_name)+strlen("libs")+20, "libs/%s",
//     lib_name); if(directory_exists(lib_dir_path) != 1)
//     {
//         mkdir(lib_dir_path,0700);
//     }
//     if (path) {
//         printf("Path for library '%s' with language '%s': %s\n", lib_name,
//         language, path);
//     } else {
//         printf("Library '%s' not found or language mismatch.\n", lib_name);
//     }

//     char *lib_name_buffer_file = malloc(strlen(INDEX_URL) + strlen(path) +
//     100); printf("path == %s\n", path); snprintf(lib_name_buffer_file,
//     strlen(INDEX_URL) + strlen(path) + 100, "%s/%s", INDEX_URL, path);
//     printf("lib_name_buffer_file: %s\n", lib_name_buffer_file);

//     char *json_data = fetch_json_data(lib_name_buffer_file);
//     if (json_data) {
//         LibraryInfo *lib_info = parse_library_json(json_data);

//         // Use lib_info as needed
//         if (lib_info) {
//             printf("Library Name: %s\n", lib_info->name);
//             printf("Git URL: %s\n", lib_info->git_url);
//             printf("Raw Path: %s\n", lib_info->raw_path);
//             printf("Source Paths:\n");
//             for (size_t i = 0; i < lib_info->src_count; i++) {
//                 printf("  %s\n", lib_info->src_paths[i]);
//             }
//             printf("Header Paths:\n");
//             for (size_t i = 0; i < lib_info->header_count; i++) {
//                 printf("  %s\n", lib_info->header_paths[i]);
//             }

//             // Save source files
//             save_source_files(lib_info);
//             save_header_files(lib_info);
//             // Free allocated memory
//             // free_library_info(lib_info);
//         }

//         free(json_data);
//     }

//     free(lib_name_buffer_file);
//     return 0;
// }

/**
 * @brief Main function to install a package.
 * 
 * This function is the entry point for installing a package. It retrieves the path
 * for the specified library and language, ensures the necessary directories exist,
 * fetches the library's JSON data, parses it, and saves the source and header files
 * to the appropriate locations.
 * 
 * @param lib_name The name of the library to install.
 * @param language The language of the library to match.
 * @return int Returns 0 if the package was successfully installed, otherwise also returns 0.
 */
int cpkg_main(char *lib_name, char *language) {
  printf("Installing package\n");
  char *path = get_lib_path(lib_name, language);
  if (directory_exists("libs") != 1) {
    mkdir("libs", 0700);
  }
  char *lib_dir_path = malloc(strlen(lib_name) + strlen("libs") + 20);
  snprintf(lib_dir_path, strlen(lib_name) + strlen("libs") + 20, "libs/%s",
           lib_name);
  if (directory_exists(lib_dir_path) != 1) {
    mkdir(lib_dir_path, 0700);
  }
  if (path) {
    printf("Path for library '%s' with language '%s': %s\n", lib_name, language,
           path);
  } else {
    printf("Library '%s' not found or language mismatch.\n", lib_name);
  }

  char *lib_name_buffer_file = malloc(strlen(INDEX_URL) + strlen(path) + 100);
  printf("path == %s\n", path);
  snprintf(lib_name_buffer_file, strlen(INDEX_URL) + strlen(path) + 100,
           "%s/%s", INDEX_URL, path);
  printf("lib_name_buffer_file: %s\n", lib_name_buffer_file);

  char *json_data = fetch_json_data(lib_name_buffer_file);
  if (json_data) {
    LibraryInfo *lib_info = parse_library_json(json_data);

    // Use lib_info as needed
    if (lib_info) {
      printf("Library Name: %s\n", lib_info->name);
      printf("Git URL: %s\n", lib_info->git_url);
      printf("Raw Path: %s\n", lib_info->raw_path);
      printf("Source Paths:\n");
      for (size_t i = 0; i < lib_info->src_count; i++) {
        printf("  %s\n", lib_info->src_paths[i]);
      }
      printf("Header Paths:\n");
      for (size_t i = 0; i < lib_info->header_count; i++) {
        printf("  %s\n", lib_info->header_paths[i]);
      }

      // Save source files
      save_source_files(lib_info);
      save_header_files(lib_info);
      free(json_data);
      free(lib_name_buffer_file);
      return 0;

      // Free allocated memory
      // free_library_info(lib_info);
    }

    free(json_data);
  }

  free(lib_name_buffer_file);
  return 0;
}