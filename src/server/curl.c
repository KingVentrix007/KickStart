#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Callback function to write data into a buffer
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    char **buffer = (char **)userp;
    
    // Reallocate memory to accommodate new data
    *buffer = realloc(*buffer, total_size + 1);
    if (*buffer == NULL) {
        fprintf(stderr, "Failed to realloc memory\n");
        return 0;
    }
    
    // Copy the data into the buffer
    memcpy(*buffer, contents, total_size);
    (*buffer)[total_size] = '\0';  // Null-terminate the buffer

    return total_size;
}

// Function to fetch data from a URL using libcurl
char *fetch_data(const char *url) {
    // printf("Fetcjing data from %s\n",url);
    CURL *curl;
    CURLcode res;
    char *data = NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        
        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(data);
            data = NULL;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return data;
}
