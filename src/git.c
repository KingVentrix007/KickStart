#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
int is_file(char *path)
{
    for (size_t i = 0; i < strlen(path); i++)
    {
        if(path[i] == '/' ||path[i] == '\\')
        {
            return -1;
        }
        else if (path[i] == '.')
        {
            return 0;
        }
        
    }
    return -1;
    
}

int ignore(char **ignore, size_t ignore_count)
{
    printf("Hello\n");
    for (size_t i = 0; i < ignore_count; i++)
    {
        char *cmd = malloc(1024);
        if(cmd == NULL)
        {
            return -1;
        }
        sprintf(cmd,"git rm --cached '%s'",ignore[i]);
        printf("cmd == %s\n",cmd);
        system(cmd);
        free(cmd);
    }
    FILE *file = fopen(".gitignore","a");
    for (size_t i = 0; i < ignore_count; i++)
    {
        if(ignore[i][0] == '.')
        {
            fprintf(file,"'*%s'\n",ignore[i]);
        }
        else if (is_file(ignore[i]) == 0)
        {
            fprintf(file,"'%s'\n",ignore[i]);
            
        }
        else
        {
            fprintf(file,"%s/\n",ignore[i]);
        }
        
        
        
    }
    fclose(file);
    return -1;
}