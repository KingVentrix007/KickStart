#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
 // For _getch()
// Structure to hold the response data
typedef struct {
    char *data;
    size_t size;
} MemoryBlock;
char *encode_url(const char *url) {
    size_t length = strlen(url);
    // Allocate memory for the encoded URL. Each special character could be encoded as %XX, so we may need more space.
    char *encoded_url = malloc(length * 3 + 1); // 3 bytes per character + 1 for the null terminator
    if (!encoded_url) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    char *ptr = encoded_url;
    for (size_t i = 0; i < length; ++i) {
        unsigned char ch = (unsigned char)url[i];
        if (ch == ' ') {
            // Encode space as %20
            *ptr++ = '%';
            *ptr++ = '2';
            *ptr++ = '0';
        } else if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~' || ch == '/' || ch == ':') {
            // Alphanumeric characters and some special characters don't need encoding
            *ptr++ = ch;
        } else {
            // Encode other special characters as %XX
            *ptr++ = '%';
            *ptr++ = "0123456789ABCDEF"[(ch >> 4) & 0x0F];
            *ptr++ = "0123456789ABCDEF"[ch & 0x0F];
        }
    }
    *ptr = '\0'; // Null-terminate the string

    return encoded_url;
}

// Callback function to write data to the MemoryBlock structure
size_t write_callback_l(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    MemoryBlock *mem = (MemoryBlock *)userp;

    char *new_data = realloc(mem->data, mem->size + total_size + 1);
    if (new_data == NULL) {
        fprintf(stderr, "Failed to expand buffer.\n");
        return 0;
    }

    mem->data = new_data;
    memcpy(&(mem->data[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->data[mem->size] = '\0'; // Null-terminate the string

    return total_size;
}

// Function to get license text from GitHub repository
char* get_license_text(const char *license_name) {
    CURL *curl;
    CURLcode res;
    char url[256];
    MemoryBlock memory = { .data = NULL, .size = 0 };

    // Map license names to their filenames
    // const char *license_file_map[] = {
    //     "Apache License 2.0", "Apache_License_2.0.txt",
    //     "Boost Software", "Boost_Software_License.txt",
    //     "GNU AFFERO GENERAL PUBLIC", "GNU_AFFERO_GENERAL_PUBLIC_LICENSE.txt",
    //     "GPLv3", "GPLv3.txt",
    //     "Unlicensed", "Unlicensed.txt",
    //     "BSD 2-Clause", "BSD_2-Clause.txt",
    //     "Creative Commons Zero v1.0 Universal", "Creative_Commons_Zero_v1.0_Universal.txt",
    //     "GNU LESSER GENERAL PUBLIC LICENSE Version 2.1", "GNU_LESSER_GENERAL_PUBLIC_LICENSE_Version_2.1.txt",
    //     "MIT", "MIT.txt",
    //     "BSD 3-Clause", "BSD_3-Clause.txt",
    //     "Eclipse Public License - v 2.0", "Eclipse_Public_License_v2.0.txt",
    //     "GPLv2", "GPLv2.txt",
    //     "Mozilla Public License Version 2.0", "Mozilla_Public_License_Version_2.0.txt"
    // };

    // // Find the filename corresponding to the license name
    // const char *filename = NULL;
    // for (int i = 0; i < sizeof(license_file_map) / sizeof(license_file_map[0]); i += 2) {
    //     if (strcmp(license_name, license_file_map[i]) == 0) {
    //         filename = license_file_map[i + 1];
    //         break;
    //     }
    // }

    // if (filename == NULL) {
    //     fprintf(stderr, "License \"%s\" not found.\n", license_name);
    //     return NULL;
    // }

    snprintf(url, sizeof(url), "https://raw.githubusercontent.com/KingVentrix007/KickStartFiles/main/LICENCE/%s", license_name);
    char *new_url = encode_url(url);
    // printf("URL:%s\n",new_url);
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize curl.\n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, new_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_l);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &memory);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(memory.data);
        memory.data = NULL;
    }

    curl_easy_cleanup(curl);

    if(strcmp(memory.data,"404: Not Found") == 0)
    {
        return NULL;
    }
    return memory.data;
}

char * get_license(const char *name) {
    const char *license_name = name;
    char *text = get_license_text(license_name);

    if (text) {
        // printf("License text:\n%s\n", text);
        return text;
        // free(text);
    }

    return NULL;
}


const char* get_license_name(int choice) {
    // Array of license names
    static const char* licenses[] = {
        "Apache License 2.0",
        "Boost Software",
        "GNU AFFERO GENERAL PUBLIC",
        "GPLv3",
        "Unlicensed",
        "BSD 2-Clause",
        "Creative Commons Zero v1.0 Universal",
        "GNU LESSER GENERAL PUBLIC LICENSE Version 2.1",
        "MIT",
        "BSD 3-Clause",
        "Eclipse Public License - v 2.0",
        "GPLv2"
    };

    // Check if the choice is within valid range
    if (choice >= 1 && choice <= 12) {
        return licenses[choice - 1];
    } else {
        return "Invalid choice";
    }
}

const char* license_menu() {
    int choice;

    // Prompt user for input
    printf("Enter a number between 1 and 12 to select a license:\n");
    printf("1: Apache License 2.0\n");
    printf("2: Boost Software\n");
    printf("3: GNU AFFERO GENERAL PUBLIC\n");
    printf("4: GPLv3\n");
    printf("5: Unlicensed\n");
    printf("6: BSD 2-Clause\n");
    printf("7: Creative Commons Zero v1.0 Universal\n");
    printf("8: GNU LESSER GENERAL PUBLIC LICENSE Version 2.1\n");
    printf("9: MIT\n");
    printf("10: BSD 3-Clause\n");
    printf("11: Eclipse Public License - v 2.0\n");
    printf("12: GPLv2\n");
    scanf("%d", &choice);

    // Get and print the license name based on user choice
    printf("Selected license: %s\n", get_license_name(choice));

    return get_license_name(choice);
}