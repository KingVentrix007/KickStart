#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include "package_manager/cpkg_main.h"
int create_template();


char* get_lang();
char* get_install();
int main_build();
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <init|template|install> [package_name]\n", argv[0]);
        printf("\tinit: Initialize a new project\n");
        printf("\ttemplate: Create a new project template\n");
        printf("\tinstall: Install a package\n");
        return 0;
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
            create_template();

        }
        
    }
    else if(strcmp(argv[1],"build") == 0)
    {
        printf("Coming soon\n");
    }
    else if (strcmp(argv[1],"run") == 0)
    {
        printf("Coming soon\n");
    }
    
    else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}