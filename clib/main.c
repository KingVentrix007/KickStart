#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "custom.h"
#define TMP_DIR "/tmp/libmanager"
#define LIBS_DIR "libs"

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    FILE *fp = (FILE *)userp;
    return fwrite(buffer, size, nmemb, fp);
}

void download_file(const char *url, const char *filename) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filename, "wb");
        if (!fp) {
            perror("fopen");
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }
}

void execute_commands(json_t *commands) {
    json_t *command;
    size_t i;

    json_array_foreach(commands, i, command) {
        const char *cmd = json_string_value(command);
        if (cmd) {
            printf("Executing: %s\n", cmd);
            system(cmd);
        }
    }
}

int clone_repository(const char *git_url, const char *dir) {
    char command[512];
    snprintf(command, sizeof(command), "git clone %s %s", git_url, dir);
    return system(command);
}

int extract_archive(const char *filename, const char *dir) {
    char command[512];
    snprintf(command, sizeof(command), "tar -xzvf %s -C %s", filename, dir);
    return system(command);
}

void build_library(const char *build_dir, json_t *build_commands) {
    chdir(build_dir);
    execute_commands(build_commands);
}

void process_library(const char *name) {
    char url[256];
    snprintf(url, sizeof(url), "https://github.com/KingVentrix007/KickStartFiles/libs/%s.json", name);

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
        return;
    }

    FILE *fp = fopen("library.json", "wb");
    if (!fp) {
        perror("fopen");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    CURLcode res = curl_easy_perform(curl);
    fclose(fp);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to download JSON file\n");
        return;
    }

    json_error_t error;
    json_t *root = json_load_file("library.json", 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return;
    }

    json_t *git_url = json_object_get(root, "git_url");
    json_t *fallback = json_object_get(root, "fallback");
    json_t *fallback_url = json_object_get(fallback, "fallback_url");
    json_t *fallback_commands = json_object_get(fallback, "fallback_commands");
    json_t *build_commands = json_object_get(root, "build_commands");

    if (json_is_string(git_url)) {
        char clone_dir[256];
        snprintf(clone_dir, sizeof(clone_dir), "%s/%s", TMP_DIR, name);

        if (clone_repository(json_string_value(git_url), clone_dir) != 0) {
            printf("Cloning failed. Attempting fallback...\n");

            if (json_is_string(fallback_url)) {
                char fallback_file[256];
                snprintf(fallback_file, sizeof(fallback_file), "%s/fallback.tar.gz", TMP_DIR);
                download_file(json_string_value(fallback_url), fallback_file);
                extract_archive(fallback_file, TMP_DIR);
                execute_commands(fallback_commands);
            }
        }

        build_library(clone_dir, build_commands);
    }

    json_decref(root);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <library_name>\n", argv[0]);
        return 1;
    }

    // Create necessary directories
    MKDIR(TMP_DIR, 0755);
    MKDIR(LIBS_DIR, 0755);

    process_library(argv[1]);

    return 0;
}
