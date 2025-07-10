#include <stdio.h>
#include "../../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32)
#endif
#ifdef __linux__
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#endif
char **hnd_find_linux(char **parms, size_t parm_count, size_t *found_file_count);

char **hnd_find(char **argv,size_t argc)
{
    #ifdef __linux__
    size_t num_found_files = 0;
    // printf("Doing alloc: %ld\n",argc);
    char **parms = malloc(argc+100);
    // printf("Done alloc\n");
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
    char **return_values =  hnd_find_linux(parms,argc-1,&num_found_files);
    for (size_t i = 1; i < argc; i++)
    {
        // printf("argv[i] %s\n",argv[i]);
        if(argv[i] == NULL)
        {
            continue;
        }
        free(parms[i-1]);
    }
    free(parms);
    return return_values;
    #else
    (void)argv;
    (void)argc;
    printf("Coming soon to windows\n");
    return NULL;
    #endif
}