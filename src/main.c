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
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        main_build();
    } else if (strcmp(argv[1], "install") == 0) {
        // printf("Package manager is not enabled at the moment\n");
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
            printf("Ill get to this, maybe\n");
        }
        else
        {   
            create_template();

        }
        
    }
     
    else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}