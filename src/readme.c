#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "curlhelp.h"


char *parse_readme_contents(char * contents)
{
    //TODO Handles all this:
    // Here's a blank template to get started. To avoid retyping too much info, do a search and replace with your text editor for the following: `github_username`, `repo_name`, `twitter_handle`, `linkedin_username`, `email_client`, `email`, `project_title`, `project_description`, `project_license`
}

int generate_readme_func(const char *readme_file_path,const char *project_name, const char *project_description, const char *project_author, const char *project_license, const char *project_version, const char *project_language, const char *project_dependencies) {
    FILE *file = fopen(readme_file_path, "w");
    if (!file) {
        perror("Failed to create README.md");
        return -1;
    }
    char *readnme_content = fetch_data("https://raw.githubusercontent.com/othneildrew/Best-README-Template/refs/heads/main/BLANK_README.md");
    if (readnme_content == NULL) {
        fprintf(stderr, "Failed to fetch README template\n");
        fclose(file);
        return -1;
    }
    fprintf(file, "%s", readnme_content);
    free(readnme_content);
    fclose(file);
    return 0;
}