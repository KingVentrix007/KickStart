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
// Callback function to write data into a buffer
//TODO Fix issue with not all data ending up in array
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    char **buffer = (char **)userp;
    
    // Check if buffer is NULL and initialize it
    if (*buffer == NULL) {
        *buffer = malloc(total_size + 1); // +1 for null terminator
        if (*buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory\n");
            return 0;
        }
        memcpy(*buffer, contents, total_size);
        (*buffer)[total_size] = '\0';  // Null-terminate the buffer
    } else {
        // Reallocate memory to append new data
        size_t current_length = strlen(*buffer);
        *buffer = realloc(*buffer, current_length + total_size + 1);
        if (*buffer == NULL) {
            fprintf(stderr, "Failed to realloc memory\n");
            return 0;
        }
        // Append the new data to the buffer
        memcpy(*buffer + current_length, contents, total_size);
        (*buffer)[current_length + total_size] = '\0';  // Null-terminate the buffer
    }

    return total_size;

}
// s
char *get_data_url(const char *url)
{
    
        CURL *curl;
        CURLcode res;
        long response_code;
        char *data = NULL;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            
            // Set the write callback function
            curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
            curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
            // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
            
            // Perform the request
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                free(data);
                data = NULL;
            }
            else
            {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if(response_code == 200) {
                    // printf("File downloaded successfully!\n");
                } else {
                    printf("Failed to download file. HTTP response code: %ld\n", response_code);
                }
            }

            // Clean up
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    
    if (data == NULL) {
        fprintf(stderr, "Failed to fetch data from URL: %s\n", url);
        return NULL;
    }
    // Save the data to the correct file path
    if(strncmp(url, LANG_BASE_URL, strlen(LANG_BASE_URL)) == 0)
    {
        char *path = malloc(strlen(url));
        if(path == NULL)
        {
            fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }
        strcpy(path,url);
        
        char *pos = strstr(path,LANG_BASE_URL);

        if (pos != NULL)
        {
            size_t len = strlen(LANG_BASE_URL);
            // Shift the remaining characters left to overwrite the substring
            memmove(pos, pos + len, strlen(pos + len) + 1);
        }
        char *full_path = malloc(strlen("/usr/local/etc/KickStart/langs")+strlen(path)+1);
        if(full_path == NULL)
        {
            fprintf(stderr, "Memory allocation failed!\n");
            free(path);
            return NULL;
        }
        sprintf(full_path,"%s%s","/usr/local/etc/KickStart/langs",path);
        // mkdir_p(full_path,0777 );
        remove_refs_heads_main(full_path); // Remove refs/heads/main from the path
        // Create the directory if it doesn't exist
        if (mkdir_p(full_path, 0777) != 0) {
            fprintf(stderr, "Failed to create directory: %s\n", full_path);
            free(path);
            free(full_path);
            return data;
        }
            FILE *fp = fopen(full_path,"w");
        if(fp == NULL)
        {
            fprintf(stderr, "Failed to create file: %s\n", full_path);
            free(path);
            free(full_path);
            return data;
        }
        
        fwrite(data,sizeof(char),strlen(data),fp);
        fclose(fp);
        
        free(path);
        free(full_path);
    }
    else
    {
        char *path = malloc(strlen(url) + 1);
        if(path == NULL)
        {
            fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }
        strcpy(path,url);
        // Extract the path from the URL, remove https://raw.githubusercontent.com
        size_t len = strlen("https://raw.githubusercontent.com/");
        if (strncmp(url, "https://raw.githubusercontent.com/", len) == 0) {
            strcpy(path, url + len); // Skip the prefix
        } else {
            strcpy(path, url); // If it doesn't start with the prefix, copy the whole URL
        }
        char *full_path = malloc(strlen("/usr/local/etc/KickStart/other/") + strlen(path) + 1);
        if(full_path == NULL)
        {
            fprintf(stderr, "Memory allocation failed!\n");
            free(path);
            return NULL;
        }
        sprintf(full_path, "%s%s", "/usr/local/etc/KickStart/other/", path);
        
        // mkdir_p(full_path,0777 );
        remove_refs_heads_main(full_path); // Remove refs/heads/main from the path
        // Create the directory if it doesn't exist
        if (mkdir_p(full_path, 0777) != 0) {
            fprintf(stderr, "Failed to create directory: %s\n", full_path);
            free(path);
            free(full_path);
            return data;
        }
        FILE *fp = fopen(full_path,"w");
        if(fp == NULL)
        {
            fprintf(stderr, "Failed to create file: %s\n", full_path);
            free(path);
            free(full_path);
            return data;
        }
        
        fwrite(data,sizeof(char),strlen(data),fp);
        fclose(fp);
        
        free(path);
        free(full_path);
    }
    return data;
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
        char *full_path = malloc(strlen("/usr/local/etc/KickStart/langs")+strlen(path));
        if(full_path == NULL)
        {
            return NULL;
        }
        sprintf(full_path,"%s%s","/usr/local/etc/KickStart/langs",path);
        // mkdir_p(full_path,0777 );
        FILE *fp = fopen(full_path,"r");
        if(fp == NULL)
        {
            printf("This template is not installed, try running offline mode WHILE connected to the internet to install the packages\n");
            FILE *fp_w = fopen(full_path,"w");
            if(fp_w == NULL)
            {
                printf("Failed to create file: %s\n",full_path);
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
    char *full_path = malloc(strlen("/usr/local/etc/KickStart/other/") + strlen(path) + 1);
    if(full_path == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        free(path);
        return NULL;
    }
    sprintf(full_path, "%s%s", "/usr/local/etc/KickStart/other/", path);
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
            printf("Failed to create file: %s\n", full_path);
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
            return handle_lang_path(url);
        } else {
            return handle_other(url);
        }
    }
    
}
