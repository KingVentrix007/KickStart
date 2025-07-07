#include <stdio.h>
#include "../../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32)

#else
#include <sys/types.h>
#include <dirent.h>
#endif
char **hnd_find_linux(char **parms,size_t parm_count,size_t *found_file_count)
{
    DIR *dp;
    struct dirent *ep;     
    dp = opendir(parms[0]);
    size_t max_files = 100;
    size_t file_count = 0;

    // Allocate initial array of char* pointers
    char **all_files = (char **)malloc(max_files * sizeof(char *));
        if (all_files == NULL) {
            fprintf(stderr, "ERROR(hnd_find_linux): Failed to allocate memory for all_files\n");
            return NULL;
        }

        if (dp != NULL) {
            while ((ep = readdir(dp)) != NULL) {
                if (file_count + 1 >= max_files) {
                    max_files *= 2; // Update max_files before realloc
                    char **temp = realloc(all_files, max_files * sizeof(char *));
                    if (temp == NULL) {
                        fprintf(stderr, "ERROR(hnd_find_linux): realloc failed\n");
                        // Clean up before exiting
                        for (size_t i = 0; i < file_count; i++) free(all_files[i]);
                        free(all_files);
                        closedir(dp);
                        return NULL;
                    }
                    all_files = temp;
                }

                all_files[file_count] = strdup(ep->d_name); // Copy file name
                if (all_files[file_count] == NULL) {
                    fprintf(stderr, "ERROR(hnd_find_linux): strdup failed\n");
                    // Clean up before exiting
                    for (size_t i = 0; i < file_count; i++) free(all_files[i]);
                    free(all_files);
                    closedir(dp);
                    return NULL;
                }

                file_count++;
            }

            closedir(dp);
            *found_file_count = file_count;
            return all_files; // Fix return type to match char ** if this is the intended return
        } else {
            perror("Couldn't open the directory");
            return NULL;
        }

}

char **hnd_find(char **argv,size_t argc)
{
    size_t num_found_files = 0;
    #if defined(WIN32)

    #else
    char *parms = malloc(argc+10);
    if(parms == NULL)
    {
        fprintf(stderr,"ERROR(hnd_find): Failed to allocate memory for parms\n");
        return NULL;
    }

    return hnd_find_linux(argv,argc,&num_found_files);
    #endif
}