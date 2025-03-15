#include "stdio.h"
#include "dirent.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"
#include <sys/stat.h>
#include <ctype.h>
#include <jansson.h>

typedef struct project_line_count
{
    size_t program_code;
    size_t comment;
    size_t blank;
    size_t config;
    size_t documentation;
}project_line_count_t;
char **comment_values;
size_t num_comments;
project_line_count_t line_break_down_project;
char **extensions;

size_t num_extensions;
const char *config_extensions[] = {
    ".json",
    ".yaml",
    ".xml",
    ".ini",
    ".env",
    ".toml",
    ".properties",
    ".conf",
    ".yml",
    ".bashrc"
};
size_t num_cfg_extensions = 10;
const char *doc_extensions[] = {
    ".md",        // Markdown
    ".rst",       // reStructuredText
    ".html",      // HTML
    ".htm",       // Alternate HTML extension
    ".pdf",       // Portable Document Format
    ".doc",       // Microsoft Word (legacy)
    ".docx",      // Microsoft Word (modern)
    ".odt",       // OpenDocument Text
    ".rtf",       // Rich Text Format
    ".tex"        // LaTeX
};
const char *build_extensions[] = {
    ".makefile",  // Makefile (GNU Make, BSD Make)
    ".mk",        // Makefile include
    ".cmake",     // CMake configuration files
    ".gradle",    // Gradle build scripts
    ".maven",     // Maven build files (usually pom.xml, but for extension reference)
    ".bazel",     // Bazel build system files
    ".ninja",     // Ninja build system files
    ".sln",       // Visual Studio Solution files
    ".vcxproj",   // Visual Studio C++ Project files
    ".build",     // Generic build script file
};
size_t num_doc_extensions = 10;
char *trim_leading_spaces(char *str) {
    while (isspace((unsigned char)*str)) str++;
    return str;
}
int starts_with_comment(const char *line) {
    // printf("line: %s",line);
    for (size_t i = 0; i < num_comments; i++) {
        size_t len = strlen(comment_values[i]);
        if (strncmp(line, comment_values[i], len) == 0) {
            return 1;
        }
    }
    return 0;
}
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot ;
}

int is_code_file(const char *ext)
{
    for (size_t i = 0; i < num_extensions; i++)
    {
        if(strcmp(ext,extensions[i]) == 0)
        {
            return 0;
        }
    }
    return -1;
    
}
int is_cfg_file(const char *ext)
{
    for (size_t i = 0; i < num_cfg_extensions; i++)
    {
        if(strcmp(ext,config_extensions[i]) == 0)
        {
            return 0;
        }
    }
    return -1;
    
}
int is_doc_file(const char *ext)
{
    for (size_t i = 0; i < num_doc_extensions; i++)
    {
        if(strcmp(ext,doc_extensions[i]) == 0)
        {
            return 0;
        }
    }
    return -1;
}

project_line_count_t *count_code_lines(char *data) {
    project_line_count_t *project = (project_line_count_t*)malloc(sizeof(project_line_count_t));
    project->blank = 0;
    project->comment = 0;
    project->program_code = 0;

    char *ptr = data;
    while (*ptr) {
        char *line_start = ptr;
        
        // Find end of the current line
        char *newline = strchr(ptr, '\n');
        if (newline) {
            *newline = '\0';  // Temporarily terminate the line
            ptr = newline + 1; // Move to the next line
        } else {
            ptr += strlen(ptr); // Move to the end of the string
        }

        char *trimmed = trim_leading_spaces(line_start);
        // printf("Line: [%s]\n", line_start);

        if (*trimmed == '\0') {
            // printf("Blank line\n");
            project->blank++;
            line_break_down_project.blank++;
        } else if (starts_with_comment(trimmed)) {
            // printf("Comment line: %s\n", trimmed);
            project->comment++;
            line_break_down_project.comment++;

        } else {
            project->program_code++;
            line_break_down_project.program_code++;
            
            // printf("Non-blank line: %s\n", trimmed);
        }
    }

    return project;
}

project_line_count_t *count_cfg_lines(char *data) {
    project_line_count_t *project = (project_line_count_t*)malloc(sizeof(project_line_count_t));
    project->blank = 0;
    project->config = 0;

    char *ptr = data;
    while (*ptr) {
        char *line_start = ptr;
        
        // Find end of the current line
        char *newline = strchr(ptr, '\n');
        if (newline) {
            *newline = '\0';  // Temporarily terminate the line
            ptr = newline + 1; // Move to the next line
        } else {
            ptr += strlen(ptr); // Move to the end of the string
        }

        char *trimmed = trim_leading_spaces(line_start);

        if (*trimmed == '\0') {
            // printf("Blank line\n");
            project->blank++;
        } else {
            project->config++;
            line_break_down_project.config++;
            // printf("Non-blank line: %s\n", trimmed);
        }
    }

    return project;
}

project_line_count_t *count_doc_lines(char *data) {
    project_line_count_t *project = (project_line_count_t*)malloc(sizeof(project_line_count_t));
    project->blank = 0;
    project->documentation = 0;

    char *ptr = data;
    while (*ptr) {
        char *line_start = ptr;
        
        // Find end of the current line
        char *newline = strchr(ptr, '\n');
        if (newline) {
            *newline = '\0';  // Temporarily terminate the line
            ptr = newline + 1; // Move to the next line
        } else {
            ptr += strlen(ptr); // Move to the end of the string
        }

        char *trimmed = trim_leading_spaces(line_start);

        if (*trimmed == '\0') {
            // printf("Blank line\n");
            project->blank++;
        } else {
            project->documentation++;
            // printf("Non-blank line: %s\n", trimmed);
            line_break_down_project.documentation++;

        }
    }

    return project;
}

char *get_filename(const char *path)
{
    char *lastPart = strrchr(path, '/');
    
    // If '/' was found, move the pointer to the next character
    if (lastPart != NULL) {
        lastPart++; 
    } else {
        strcpy(lastPart,path);
        // lastPart = path; // No '/' found, so the whole string is the last part
    }
    return lastPart;
}
int count_lines(char *path)
{
    FILE *file = fopen(path,"r");
    if(file == NULL)
    {
        perror("Cant open file");
        return -1;
    }
    fseek(file,0,SEEK_END);
    size_t size =  ftell(file);
    rewind(file);
    char *data = malloc(size+100);
    
    if(data == NULL)
    {
        printf("\n---------------------------------\n");
        printf("size: %zu\n",size);
        printf("PATH: %s\n",path);
        perror("Can't allocate memory(-2)");
        printf("---------------------------------\n");

        return -2;
    }
    fread(data,1,size,file);
    const char *ext = get_filename_ext(path);
    if(ext[0] != '.')
    {
        
    }
    else
    {
        FILE *file = fopen("kpm_lines.txt","a");
        char *filename = get_filename(path);
        if(file == NULL)
        {
            perror("Cant open file");
            return -3;
        }
        if(is_code_file(ext) == 0)
        {
            project_line_count_t *project = count_code_lines(data);
            fprintf(file, "[%s]:\n    Code:[%zu]\n    Blank:[%zu]\n    Comment:[%zu]\n", filename, project->program_code, project->blank, project->comment);
        }
        else if (is_cfg_file(ext) == 0)
        {
            project_line_count_t *project = count_cfg_lines(data);
            fprintf(file, "[%s]\n    Config:[%zu]\n    Blank:[%zu]\n", filename, project->config, project->blank);
        }
        else if (is_doc_file(ext) == 0)
        {
            project_line_count_t *project = count_doc_lines(data);
            fprintf(file, "[%s]\n    Doc:[%zu]\n    Blank:[%zu]\n", filename, project->documentation,project->blank);

        }
        


        
        
        

    }
    return 0;

}

void traverse_directory(const char *dir_path) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dir = opendir(dir_path);

    if (!dir) {
        perror("opendir failed");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.') {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        

        if (stat(full_path, &statbuf) == -1) {
            perror("stat failed");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // Recursively traverse subdirectories
            traverse_directory(full_path);
        } else {
            // Print file path
            // printf("%s\n", full_path);
            count_lines(full_path);
        }
    }

    closedir(dir);
}


int do_line_count(char **extensions_in,size_t num_extensions_in,char **comments_in,size_t comments_in_count)
{
    num_extensions = num_extensions_in;  // Number of extensions
    extensions = malloc(num_extensions * sizeof(char *)); // Allocate array of pointers
    for (size_t i = 0; i < num_extensions; i++)
    {
        extensions[i] = strdup(extensions_in[i]);
    }
    num_comments = comments_in_count;
    comment_values = malloc(num_comments *sizeof(char *));
    for (size_t i = 0; i < num_comments; i++)
    {
       comment_values[i] = strdup(comments_in[i]);
    }
    
    traverse_directory(".");
    printf("Total lines of code: [%zu]\n",line_break_down_project.program_code);
    printf("Total blank lines: [%zu]\n",line_break_down_project.blank);
    printf("Total code comments: [%zu]\n",line_break_down_project.comment);
    printf("Total config lines: [%zu]\n",line_break_down_project.config);
    printf("Total documentation lines: [%zu]\n",line_break_down_project.documentation);
    return 0;
}

char **get_comments_linecount(size_t *num_comments)
{
    const char *filename = "project.json";
    json_error_t error;
    json_t *root;
    json_t *comments;
    char *comment_values_local;

    // Load and parse the JSON file
    root = json_load_file(filename, 0, &error);
    if(!root) {
        fprintf(stderr, "Error loading JSON file: %s\n", error.text);
        return NULL;
    }

    // Extract the "language" value from the JSON data
    comments = json_object_get(root, "comments");
    if(!json_is_string(comments)) {
        fprintf(stderr, "Error: comments field is missing or not a string\n");
        json_decref(root);
        return NULL;
    }
    comment_values_local = strdup(json_string_value(comments));
    size_t num_comments_local = 0;
    for (size_t i = 0; i < strlen(comment_values_local); i++)
    {
        if(comment_values_local[i] == ',')
        {
            num_comments_local++;
        }
    }
    char **comments_out = malloc(num_comments_local+10*sizeof(char *));
    char *token = strtok(comment_values_local, ",");
    size_t count_local = 0;
    while (token != NULL)
    {

        comments_out[count_local] = malloc(strlen(token)+5);
        comments_out[count_local] = strdup(token);
        token = strtok(NULL, ",");
        count_local++;

    }
    *num_comments = count_local;
    return comments_out;
    
}

char **get_ext_linecount(size_t *num_ext)
{
    const char *filename = "project.json";
    json_error_t error;
    json_t *root;
    json_t *ext;
    char **local_ext;

    // Load and parse the JSON file
    root = json_load_file(filename, 0, &error);
    if(!root) {
        fprintf(stderr, "Error loading JSON file: %s\n", error.text);
        return NULL;
    }
    ext = json_object_get(root,"extensions");
    if (!json_is_array(ext))
    {
        fprintf(stderr, "Error: 'words' is not an array\n");
        json_decref(root);
        return NULL;
    }
    size_t index;
    json_t *value;
    printf("Words in JSON:\n");
    size_t count = 0;
    json_array_foreach(ext, index, value) {
        if (json_is_string(value)) {
            count++;
        }
    }
    local_ext = (char **)malloc((count+1)*sizeof(char *));
    json_array_foreach(ext, index, value) {
        if (json_is_string(value)) {
            local_ext[index] = strdup(json_string_value(value));
        }
    }
    *num_ext = count;
    return local_ext;
    json_decref(root);
}
int get_line_count()
{
    char **local_comments = get_comments_linecount(&num_comments);
    comment_values = malloc(num_comments * sizeof(char *));
    for (size_t i = 0; i < num_comments; i++)
    {
        comment_values[i] = strdup(local_comments[i]);
    }
    char **local_ext = get_ext_linecount(&num_extensions);
    extensions = malloc(num_comments * sizeof(char *));
    for (size_t i = 0; i < num_extensions; i++)
    {
        extensions[i] = strdup(local_ext[i]);
    }
    traverse_directory(".");
    printf("Total lines of code: [%zu]\n",line_break_down_project.program_code);
    printf("Total blank lines: [%zu]\n",line_break_down_project.blank);
    printf("Total code comments: [%zu]\n",line_break_down_project.comment);
    printf("Total config lines: [%zu]\n",line_break_down_project.config);
    printf("Total documentation lines: [%zu]\n",line_break_down_project.documentation);
    
    return 0;
}