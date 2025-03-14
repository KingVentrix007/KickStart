#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "stdbool.h"
#include "../templates/custom.h"
bool is_offline();
char *get_data_url(const char *url);
int mkdir_p(const char *path, mode_t mode) ;
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

    return data;
}

// Function to fetch data from a URL using libcurl

char *fetch_data(const char *url) {
    // printf("Fetcjing data from %s\n",url);
    if(is_offline() == false)
    {
        return get_data_url(url);
    }
    else
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
        fread(buffer,sizeof(char),size,fp);
        fclose(fp);
        return buffer;
    }
    
}
