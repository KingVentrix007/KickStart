#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include "jansson.h"
// #include "fetch.h"
#include <libgen.h>
#include <dirent.h>
#include "errno.h"

char* get_lang();
int try_make()
{
    FILE *makefile_fp = fopen("makefile","r");
    if(makefile_fp == NULL)
    {
        return -1;
    }
    fclose(makefile_fp);
    return 0;
}
int try_bash()
{
    FILE *bash_fp = fopen("build.sh","r");
    if(bash_fp == NULL)
    {
        return -1;
    } 
    fclose(bash_fp); 
    return 0;
}
int run_project()
{
   if(try_make() == 0)
   {
    if(system("make") == 0)
    {
        system("make run");
    }
    else
    {
        printf("Error running with make");
    }
    
   }
   return 0;
}