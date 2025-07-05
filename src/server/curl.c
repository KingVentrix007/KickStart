#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "stdbool.h"
#include "../templates/custom.h"
bool is_offline();
char *get_data_url(const char *url);
int mkdir_p(const char *path, mode_t mode) ;
void remove_refs_heads_main(char *str);
struct MemoryStructC {
    char *memory;
    size_t size;
};

// Callback function to write data into a buffer
//TODO Fix issue with not all data ending up in array
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStructC *mem = (struct MemoryStructC *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}

// s
char *get_data_url(const char *url)
{
        
        CURL *curl;
    CURLcode res;
    long response_code;

    struct MemoryStructC chunk = {
        .memory = malloc(1),
        .size = 0
    };

    if (chunk.memory == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            chunk.memory = NULL;
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != 200) {
                printf("Failed to download file. HTTP response code: %ld\n", response_code);
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    if (chunk.memory == NULL) {
        fprintf(stderr, "Failed to fetch data from URL: %s\n", url);
        return NULL;
    }

    // ----------- FILE PATH CONSTRUCTION AND SAVE LOGIC -----------
    char *path = malloc(strlen(url) + 100);
    if (path == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        free(chunk.memory);
        return NULL;
    }

    const char *home = getenv("HOME");

    if (strncmp(url, LANG_BASE_URL, strlen(LANG_BASE_URL)) == 0) {
        // Case: LANG_BASE_URL path
        strcpy(path, url);
        char *pos = strstr(path, LANG_BASE_URL);
        if (pos) {
            memmove(pos, pos + strlen(LANG_BASE_URL), strlen(pos + strlen(LANG_BASE_URL)) + 1);
        }

        char *full_path = malloc(strlen(home) + strlen(path) + 100);
        if (!full_path) {
            fprintf(stderr, "Memory allocation failed!\n");
            free(path);
            free(chunk.memory);
            return NULL;
        }

        sprintf(full_path, "%s/.local/share/KickStart/langs%s", home, path);
        remove_refs_heads_main(full_path);

        char *dir_path = strdup(full_path);
        char *last_slash = strrchr(dir_path, '/');
        if (last_slash) {
            *last_slash = '\0';
            printf("Creating dir at %s\n", dir_path);
            if (mkdir_p(dir_path, 0777) != 0) {
                fprintf(stderr, "Failed to create directory: %s\n", full_path);
                free(path); free(full_path); free(dir_path); free(chunk.memory);
                return NULL;
            }
        }
        free(dir_path);

        FILE *fp = fopen(full_path, "w");
        if (!fp) {
            fprintf(stderr, "Failed to create file(get_data_url): %s\n", full_path);
            free(path); free(full_path); free(chunk.memory);
            return NULL;
        }

        fwrite(chunk.memory, sizeof(char), chunk.size, fp);
        fclose(fp);

        free(path);
        free(full_path);
    } else {
        // Case: Fallback for unknown (other) URL
        size_t base_len = strlen("https://raw.githubusercontent.com/");
        if (strncmp(url, "https://raw.githubusercontent.com/", base_len) == 0) {
            strcpy(path, url + base_len);
        } else {
            strcpy(path, url);
        }

        char *full_path = malloc(strlen(home) + strlen(path) + 100);
        if (!full_path) {
            fprintf(stderr, "Memory allocation failed!\n");
            free(path); free(chunk.memory);
            return NULL;
        }

        sprintf(full_path, "%s/.local/share/KickStart/other/%s", home, path);
        remove_refs_heads_main(full_path);

        char *dir_path = strdup(full_path);
        char *last_slash = strrchr(dir_path, '/');
        if (last_slash) {
            *last_slash = '\0';
            printf("Creating dir at %s\n", dir_path);
            if (mkdir_p(dir_path, 0777) != 0) {
                fprintf(stderr, "Failed to create directory: %s\n", full_path);
                free(path); free(full_path); free(dir_path); free(chunk.memory);
                return NULL;
            }
        }
        free(dir_path);

        FILE *fp = fopen(full_path, "w");
        if (!fp) {
            fprintf(stderr, "Failed to create file(get_data_url_2): %s\n", full_path);
            free(path); free(full_path); free(chunk.memory);
            return NULL;
        }

        fwrite(chunk.memory, sizeof(char), chunk.size, fp);
        fclose(fp);

        free(path);
        free(full_path);
    }

    return chunk.memory;  // ✅ caller must free this
}



// Function to fetch data from a URL using libcurl
char *handle_lang_path(const char *url)
{
    char *path = malloc(strlen(url));
        strcpy(path,url);
        
        char *pos = strstr(path,LANG_BASE_URL);

        if (pos != NULL)
        {
            size_t len = strlen(LANG_BASE_URL);
            // Shift the remaining characters left to overwrite the substring
            memmove(pos, pos + len, strlen(pos + len) + 1);
        }
        char *full_path = malloc(strlen("local/share/KickStart/langs")+strlen(path)+100);
        if(full_path == NULL)
        {
            return NULL;
        }
        const char *home = getenv("HOME");
        
        sprintf(full_path,"%s/.%s%s",home,"local/share/KickStart/langs",path);

        // Duplicate to get directory path only
//         char *dir_path = strdup(full_path);
//         char *last_slash = strrchr(dir_path, '/');
//         if (last_slash) {
//             *last_slash = '\0'; // Truncate to get directory path
//             mkdir_p(dir_path, 0777); // Only create the directories
//         }
// free(dir_path);
        FILE *fp = fopen(full_path,"r");
        if(fp == NULL)
        {
            printf("This template is not installed, try running offline mode WHILE connected to the internet to install the packages\n");
            FILE *fp_w = fopen(full_path,"w");
            if(fp_w == NULL)
            {
                printf("Failed to create file(handle_lang_path): %s\n",full_path);
            }
            char *data = get_data_url(url);
            fwrite(data,sizeof(char),strlen(data),fp_w);
            fclose(fp_w);
            // printf("data == %s\n",data);
            return data;

        }
        fseek(fp,0L,SEEK_END);

        size_t size = ftell(fp);
        rewind(fp);
        char *buffer = malloc(size+100);
        if(buffer == NULL)
        {
            return NULL;
        }
        memset(buffer, 0, size + 100); // Initialize buffer to zero
        fread(buffer,sizeof(char),size,fp);
        fclose(fp);
        return buffer;
}
void remove_refs_heads_main(char *str) {
    const char *to_remove = "refs/heads/main";
    char *pos = strstr(str, to_remove);
    
    if (pos) {
        size_t len = strlen(to_remove);
        memmove(pos, pos + len, strlen(pos + len) + 1); // shift remaining string left
    }
}
char *handle_other(const char *url)
{
    char *path = malloc(strlen(url));
    if(path == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    };
    //Extract the path from the URL, remove https://raw.githubusercontent.com
    size_t len = strlen("https://raw.githubusercontent.com/");
    if (strncmp(url, "https://raw.githubusercontent.com/",len) == 0) {
        strcpy(path, url + len); // Skip the 
    }
    else {
        strcpy(path, url); // If it doesn't start with the prefix, copy the whole URL
    }
    char *full_path = malloc(strlen("local/share/KickStart/other/") + strlen(path) + 100);
    if(full_path == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        free(path);
        return NULL;
    }
    char *home = getenv("HOME");
    sprintf(full_path, "%s/.%s%s",home, "local/share/KickStart/other/", path);
    remove_refs_heads_main(full_path); // Remove refs/heads/main from the path
    // Create the directory if it doesn't exist
    if (mkdir_p(full_path, 0777) != 0) {
        fprintf(stderr, "Failed to create directory: %s\n", full_path);
        free(path);
        free(full_path);
        return NULL;
    }
    //Now read the file from the full path
    FILE *fp = fopen(full_path, "r");
    if(fp == NULL)
    {
        printf("This template is not installed, try running offline mode WHILE connected to the internet to install the packages\n");
        FILE *fp_w = fopen(full_path, "w");
        if(fp_w == NULL)
        {
            printf("Failed to create file(handle_other): %s\n", full_path);
            free(path);
            free(full_path);
            return NULL;
        }
        char *data = get_data_url(url);
        fwrite(data, sizeof(char), strlen(data), fp_w);
        fclose(fp_w);
        free(path);
        free(full_path);
        return data;
    }
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    char *buffer = malloc(size + 100);
    if(buffer == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        fclose(fp);
        free(path);
        free(full_path);
        return NULL;
    }
    fread(buffer, sizeof(char), size, fp);
    fclose(fp);
    free(path);
    free(full_path);
    return buffer;
}
char *fetch_data(const char *url) {
    // printf("Fetcjing data from %s\n",url);
    if(is_offline() == false)
    {
        return get_data_url(url);
    }
    else
    {
        if(strncmp(url, LANG_BASE_URL, strlen(LANG_BASE_URL)) == 0) {
            // If the URL starts with LANG_BASE_URL, handle it as a language path
            printf("Fetching data from lang URL: %s\n",url);
            return handle_lang_path(url);
        } else {
            printf("Fetching data from other URL: %s\n", url);
            return handle_other(url);
        }
    }
    
}
