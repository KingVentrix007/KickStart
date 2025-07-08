#include <stdio.h>
#include "../../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32)

#else
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#endif
char **hnd_find_linux(char **parms, size_t parm_count, size_t *found_file_count)
{
    DIR *dp;
    struct dirent *ep;     
    dp = opendir(parms[0]);
    size_t max_files = 1000;
    size_t file_count = 0;

    char **all_files = (char **)malloc(max_files * sizeof(char *));
    if (all_files == NULL) {
        fprintf(stderr, "ERROR(hnd_find_linux): Failed to allocate memory for all_files\n");
        return NULL;
    }

    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            const char *name = ep->d_name;

            // Skip current/parent dirs
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
                continue;

            int matched = 0;

            if (parm_count <= 1) {
                // No pattern provided, match everything
                matched = 1;
            } else {
                // Try matching against all patterns
                for (size_t i = 1; i < parm_count; i++) {
                    if (fnmatch(parms[i], name, 0) == 0) {
                        matched = 1;
                        break;
                    }
                }
            }

            if (!matched)
                continue;

            if (file_count + 1 >= max_files) {
                max_files *= 2;
                char **temp = realloc(all_files, max_files * sizeof(char *));
                if (temp == NULL) {
                    fprintf(stderr, "ERROR(hnd_find_linux): realloc failed\n");
                    for (size_t i = 0; i < file_count; i++) free(all_files[i]);
                    free(all_files);
                    closedir(dp);
                    return NULL;
                }
                all_files = temp;
            }

            all_files[file_count] = strdup(name);
            if (all_files[file_count] == NULL) {
                fprintf(stderr, "ERROR(hnd_find_linux): strdup failed\n");
                for (size_t i = 0; i < file_count; i++) free(all_files[i]);
                free(all_files);
                closedir(dp);
                return NULL;
            }

            file_count++;
        }

        closedir(dp);
        *found_file_count = file_count;
        all_files[file_count] = NULL;  // Null-terminate array
        return all_files;
    } else {
        printf("Directory '%s' couldn't be opened\n", parms[0]);
        perror("Couldn't open the directory");
        return NULL;
    }
}

char **hnd_find(char **argv,size_t argc)
{
    size_t num_found_files = 0;
    #if defined(WIN32)

    #else
    char **parms = malloc(argc+10);
    if(parms == NULL)
    {
        fprintf(stderr,"ERROR(hnd_find): Failed to allocate memory for parms\n");
        return NULL;
    }
    for (size_t i = 1; i < argc; i++)
    {
        // printf("argv[i] %s\n",argv[i]);
        if(argv[i] == NULL)
        {
            continue;
        }
        parms[i-1] = strdup(argv[i]);
    }
    // printf("Calling functions\n");
    return hnd_find_linux(parms,argc-1,&num_found_files);
    #endif
}