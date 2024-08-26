#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "package_manager/package_manager.h"


char* get_lang();
char* get_lang() {
    return "c";  // Will be changed to use json
}
int main_build();
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <init|install> [package_name]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        main_build();
    } else if (strcmp(argv[1], "install") == 0) {
        printf("Package manager is not enabled at the moment\n");
        return 0;
        if (argc < 3) {
            fprintf(stderr, "Usage: %s install <package_name>\n", argv[0]);
            return 1;
        }

        char *lang = get_lang();
        if (strcmp(lang, "c") == 0) {
            install_package_c(argv[2]);
        } else {
            fprintf(stderr, "Unsupported language: %s\n", lang);
            return 1;
        }
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}