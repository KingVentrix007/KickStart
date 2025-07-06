#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "curlhelp.h"
#define MAX_LEN 1024
char *resolve_description(const char *desc, const char *project_title) {
    static char buffer[2048]; // adjust size as needed
    char *dst = buffer;
    const char *src = desc;

    while (*src) {
        if (strncmp(src, "project_title", 13) == 0) {
            dst += sprintf(dst, "%s", project_title);
            src += 13;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
    return buffer;
}

char *parse_readme_contents(char * contents,const char *github_username_in,const char *project_name_in, const char *project_description, const char *project_license,const char **dependencies)
{
     char github_username[MAX_LEN];
    char project_name[MAX_LEN];
    char temp_input[MAX_LEN];
    memset(temp_input, 0, sizeof(temp_input));
    strcpy(temp_input,"n");
    getc(stdin); // Clear the newline character from the input buffer
    // Copy inputs as default
    strncpy(github_username, github_username_in, MAX_LEN);
    strncpy(project_name, project_name_in, MAX_LEN);
    
    // Ask user to confirm GitHub username
    printf("Your current GitHub username is: %s\n", github_username_in);
    printf("Do you want to change it(n)? (y/n): ");
    fgets(temp_input, sizeof(temp_input), stdin);
    if (temp_input[0] == 'y' || temp_input[0] == 'Y') {
        printf("Enter new GitHub username: ");
        fgets(github_username, sizeof(github_username), stdin);
        github_username[strcspn(github_username, "\n")] = '\0'; // remove newline
    }

    // Ask user to confirm repo name
    printf("Your current repo/project name is: %s\n", project_name_in);
    printf("Do you want to change it(n)? (y/n): ");
    fgets(temp_input, sizeof(temp_input), stdin);
    if (temp_input[0] == 'y' || temp_input[0] == 'Y') {
        printf("Enter new repo/project name: ");
        fgets(project_name, sizeof(project_name), stdin);
        project_name[strcspn(project_name, "\n")] = '\0'; // remove newline
    }
    //TODO Handles all this:
    // Here's a blank template to get started. To avoid retyping too much info, do a search and replace with your text editor for the following: `github_username`, `repo_name`, `twitter_handle`, `linkedin_username`, `email_client`, `email`, `project_title`, `project_description`, `project_license`
    // char github_username[1024] = "github_username";
    // char repo_name[1024] = "repo_name";
    char twitter_handle[1024] = "twitter_handle";
    char linkedin_username[1024] = "linkedin_username";
    char email_client[1024] = "email_client";
    char email[1024] = "email";
    // char project_title[1024] = project_name;
    // char project_description[4096] = project_description;
    // char project_license[1024] = project_license;
    //Get github username
    // printf("Enter your GitHub username [default: %s]: ", github_username);
    
    //Get repo name: Is retrieved from the project name

    //Get twitter handle
    // getc(stdin); // Clear the newline character from the input buffer
    printf("Enter your Twitter handle [default: %s]: ", twitter_handle);
    fgets(twitter_handle, sizeof(twitter_handle), stdin);
    if (twitter_handle[0] == '\n') {
        strcpy(twitter_handle, "twitter_handle");
    } else {
        twitter_handle[strcspn(twitter_handle, "\n")] = 0;  // Remove trailing newline
    }
    //Get linkedin username
    printf("Enter your LinkedIn username [default: %s]: ", linkedin_username);
    fgets(linkedin_username, sizeof(linkedin_username), stdin);
    if (linkedin_username[0] == '\n') {
        strcpy(linkedin_username, "linkedin_username");
    } else {
        linkedin_username[strcspn(linkedin_username, "\n")] = 0;  // Remove trailing newline
    }
    //Get email client
    printf("Enter your email client [default: %s]: ", email_client);
    fgets(email_client, sizeof(email_client), stdin);
    if (email_client[0] == '\n') {
        strcpy(email_client, "email_client");
    } else {
        email_client[strcspn(email_client, "\n")] = 0;  // Remove trailing newline
    }
    //Get email
    printf("Enter your email [default: %s]: ", email);
    fgets(email, sizeof(email), stdin);
    if (email[0] == '\n') {
        strcpy(email, "email");
    } else {
        email[strcspn(email, "\n")] = 0;  // Remove trailing newline
    }
    if(project_description == NULL || strlen(project_description) == 0)
    {
        char *generic_descoption = "project_title is a modern and flexible software solution designed to streamline development workflows and improve productivity. Built with scalability and maintainability in mind, this project provides a solid foundation for developers looking to build efficient, reliable, and secure applications. Whether you're prototyping a new idea or integrating it into a larger system, project_title offers clean architecture, well-documented code, and customizable features to support a wide range of use cases.";
        project_description = resolve_description(generic_descoption,project_name);

    }
    char *new_contents = malloc(strlen(contents) * 2 + 1024); // overshoot instead of estimate
    if (!new_contents) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    char *src = contents;
    char *dst = new_contents;
    for (size_t i = 0; i < 2; i++)
    {
        /* code */
    }
    
    while (*src) {
    if (strncmp(src,"Here's a blank template",strlen("Here's a blank template")) == 0)
    {
        dst += sprintf(dst, "%s", "This is where you will write about your project, its features, and how to use it");
        src+=strlen("Here's a blank template to get started. To avoid retyping too much info, do a search and replace with your text editor for the following: `github_username`, `repo_name`, `twitter_handle`, `linkedin_username`, `email_client`, `email`, `project_title`, `project_description`, `project_license`");
    }
    if (strncmp(src, "github_username", 15) == 0) {
        dst += sprintf(dst, "%s", github_username);
        src += 15;
    }
     
    else if (strncmp(src, "repo_name", 9) == 0) {
        dst += sprintf(dst, "%s", project_name); // or use actual repo_name
        src += 9;
    } else if (strncmp(src, "twitter_handle", 14) == 0) {
        dst += sprintf(dst, "%s", twitter_handle);
        src += 14;
    } else if (strncmp(src, "linkedin_username", 17) == 0) {
        dst += sprintf(dst, "%s", linkedin_username);
        src += 17;
    } else if (strncmp(src, "email_client", 12) == 0) {
        dst += sprintf(dst, "%s", email_client);
        src += 12;
    } else if (strncmp(src, "email", 5) == 0) {
        dst += sprintf(dst, "%s", email);
        src += 5;
    } else if (strncmp(src, "project_title", 13) == 0) {
        dst += sprintf(dst, "%s", project_name);
        src += 13;
    
    } else if (strncmp(src, "project_description", 19) == 0) {
        // printf("Project description: [%s]\n", project_description);
        if(project_description == NULL || strlen(project_description) == 0) {
            dst += sprintf(dst, "%s","No description provided");
        }
        else
        {
            dst += sprintf(dst, "%s", project_description);
        }
        // printf("Project description after check: [%s]\n", dst);
        src += 19;
    } else if (strncmp(src, "project_license", 15) == 0) {
        dst += sprintf(dst, "%s", project_license);
        src += 15;
    }else if (strncmp(src,"### Built With",14) == 0)
    {
        dst += sprintf(dst, "### Built With\n%s", "* [![Kickstart](https://github.com/KingVentrix007/KickStart/raw/master/images/badge.png)](https://github.com/KingVentrix007/KickStart)");
        src += 14;
    }    
     
    else {
        *dst++ = *src++;
    }
}
*dst = '\0'; // null-terminate
return new_contents;


}


int generate_readme_func(const char *readme_file_path,const char *project_name, const char *project_description, const char *project_author, const char *project_license, const char *project_version, const char *project_language, const char **project_dependencies) {
    FILE *file = fopen(readme_file_path, "w");
    printf("Generating README.md at %s\n", readme_file_path);
    // printf("Version: %s\n", project_version);
    // printf("Language: %s\n", project_language);
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
    char *parsed_content = parse_readme_contents(readnme_content,project_author, project_name, project_description, project_license, project_dependencies);
    if (parsed_content == NULL) {
        fprintf(stderr, "Failed to parse README contents\n");
        free(readnme_content);
        fclose(file);
        return -1;
    }
    fprintf(file, "%s", parsed_content);
    // printf("%s\n", parsed_content);
    free(parsed_content);
    printf("README.md generated successfully at %s\n", readme_file_path);
    printf("You can now edit it to add more information about your project\n");
    printf("If you want to add more information, you can edit the file at %s\n", readme_file_path);
    free(readnme_content);
    fclose(file);
    return 0;
}