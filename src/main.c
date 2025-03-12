#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include "package_manager/cpkg_main.h"
#include "run/run.h"
#include "stdbool.h"

bool offline = false;
bool is_offline()
{
    return offline;
}

int create_template();


char* get_lang();
char* get_install();
int main_build();
int main(int argc, char **argv) {
    
    printf("kpm compile date: %s\n",__TIME__);
    if (argc < 2) {
        printf("Usage: %s <init|template|install|run|build> [package_name]\n", argv[0]);
        printf("\tinit: Initialize a new project\n");
        printf("\ttemplate: Create a new project template\n");
        printf("\tinstall: Install a package\n");
        printf("\trun: Runs the project\n");
        printf("\tbuild: Builds the project\n");
        return 0;
    }
    if(strcmp(argv[argc-1],"-o") == 0)
    {
        printf("WARNING: offline mode is VERY bugged at the moment\n");
        offline = true;
    }
    if (strcmp(argv[1], "init") == 0) {
        main_build();
    } else if (strcmp(argv[1], "install") == 0) {
        printf("Package manager DOSE NOT support big packages like libcurl or jansson\n");
        printf("In addition, due to a shortage of time. There are very,very few libraries supported, for languages like c, I recommend using clib(https://github.com/clibs/clib)\n");
        printf("For other languages, I recommend looking for a library manger for that language\n");


        // return 0;
        if (argc < 3) {
            fprintf(stderr, "Usage: %s install <package_name>\n", argv[0]);
            return 1;
        }

        char *lang = get_lang();
        char *install_cmd = get_install();
        if (strcmp(lang, "c") == 0) {
            cpkg_main(argv[2], lang);
        } else if (strcmp(install_cmd,"(null)") == 0)
        {
            cpkg_main(argv[2], lang);
        }   
        else
        {
            char *command = malloc(strlen(argv[2])+strlen(install_cmd)+50);
            snprintf(command,strlen(argv[2])+strlen(install_cmd)+50,"%s %s",install_cmd,argv[2]);
            system(command);
            free(command);
            // fprintf(stderr, "Unsupported language: %s\n", lang);
            // return 1;
        }
    }else if (strcmp(argv[1],"template") == 0)
    {
        if(argc >= 3 && strcmp(argv[2],"-f") == 0)
        {
            printf("This feature is coming soon. It will build the json file from the contents of the current folder\n");
        }
        else
        {   
            printf("--------------------------------------------------------\nWARNING\n\nTemplate creation code is outdated\n");
            create_template();

        }
        
    }
    else if(strcmp(argv[1],"build") == 0)
    {
        build_project();
    }
    else if (strcmp(argv[1],"run") == 0)
    {
        // printf("Coming soon\n");
        run_project();
    }
    
    else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}