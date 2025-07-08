#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <unistd.h>

char **hnd_find_linux_internal(const char *dir_path, char **patterns, size_t pattern_count,
                               size_t *file_count, size_t *max_files,
                               char ***file_list_ptr, int recursive);

char **hnd_find_linux(char **parms_in, size_t parm_count, size_t *found_file_count)
{
    // printf("Doing alloc for parms\n");
    char **parms = malloc((parm_count + 1) * sizeof(char *));
    // printf("Done parm alloc\n");
    for (size_t i = 0; i < parm_count; i++)
    {
        parms[i] = strdup(parms_in[i]);
    }
    
    // printf("Starting seach: %s\n",parms[1]);
    int recursive = 0;
    size_t pattern_start = 1;

    if (parm_count > 1 && strcmp(parms[1], "-r") == 0) {
        // printf("Is rec\n");
        recursive = 1;
        pattern_start = 2;
    }
    // printf("Setting patterns\n");
    size_t pattern_count = parm_count - pattern_start;
    // printf("pattern_start = %zu, parm_count = %zu\n", pattern_start, parm_count);
// for (size_t i = 0; i < parm_count; i++) {
//     printf("parms[%zu] = %s\n", i, parms[i]);
// }
//     printf("pattern_count == %ld\n",pattern_count);
    char **patterns = malloc((pattern_count+100) * sizeof(char *));
    if (!patterns) {
        fprintf(stderr, "ERROR: Failed to allocate patterns array\n");
        return NULL;
    }
    // printf("Test allocation\n");
    // char *temp_alloc2 = malloc(1000);
    // printf("Doing value sets\n");
    for (size_t i = 0; i < pattern_count; i++) {
        patterns[i] = strdup(parms[pattern_start + i]);
    }
    // char *temp_alloc = malloc(1000);
    size_t file_count = 0;
    size_t max_files = 24;
    // printf("max_files = %zu\n", max_files);
    // printf("Doing allocation\n");
    char **all_files = malloc(max_files * sizeof(char *));


    // printf("Allocation complete\n");
    if (!all_files) {
        fprintf(stderr, "ERROR(hnd_find_linux): Failed to allocate memory\n");
        return NULL;
    }
    // printf("Calling internal\n");
    if (!hnd_find_linux_internal(parms[0], patterns, pattern_count, &file_count, &max_files, &all_files, recursive)) {
        fprintf(stderr,"ERROR(hnd_find_linux): Call to hnd_find_linux internal failed\n");
        for (size_t i = 0; i < file_count; i++) free(all_files[i]);
        free(all_files);
        return NULL;
    }

    *found_file_count = file_count;
    all_files[file_count] = NULL;  // Null-terminate
    for (size_t i = 0; i < parm_count; i++)
    {
       free( parms[i]);
    }
    free(parms);
    return all_files;
}

char **hnd_find_linux_internal(const char *dir_path, char **patterns, size_t pattern_count,
                               size_t *file_count, size_t *max_files,
                               char ***file_list_ptr, int recursive)
{
    // printf("Opening dir\n");
    DIR *dp = opendir(dir_path);
    if (!dp) {
        perror("Couldn't open directory");
        return NULL;
    }

    struct dirent *ep;
    while ((ep = readdir(dp)) != NULL) {
        const char *name = ep->d_name;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            continue;

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, name);

        struct stat statbuf;
        if (stat(full_path, &statbuf) != 0)
            continue;

        if (S_ISDIR(statbuf.st_mode) && recursive) {
            if (!hnd_find_linux_internal(full_path, patterns, pattern_count, file_count, max_files, file_list_ptr, recursive)) {
                closedir(dp);
                return NULL;
            }
            continue;
        }

        int matched = 0;
        if (pattern_count == 0) {
            matched = 1;
        } else {
            for (size_t i = 0; i < pattern_count; i++) {
                if (fnmatch(patterns[i], name, 0) == 0) {
                    matched = 1;
                    break;
                }
            }
        }

        if (!matched)
            continue;

        // Get the current list
        char **file_list = *file_list_ptr;

        if (*file_count + 1 >= *max_files) {
            *max_files *= 2;
            // printf("Call realoc\n");
            char **temp = realloc(file_list, (*max_files) * sizeof(char *));
            if (!temp) {
                fprintf(stderr, "ERROR: realloc failed\n");
                closedir(dp);
                return NULL;
            }
            *file_list_ptr = temp;
            file_list = temp;
        }

        file_list[*file_count] = strdup(full_path);
        if (!file_list[*file_count]) {
            fprintf(stderr, "ERROR: strdup failed\n");
            closedir(dp);
            return NULL;
        }

        (*file_count)++;
    }

    closedir(dp);
    return *file_list_ptr;
}
