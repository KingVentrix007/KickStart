#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *get_variable_value(const char *name);

int hnd_echo(char **argv, size_t argc)
{
    for (size_t i = 1; i < argc; i++) {
        char *to_echo = argv[i];

        if (to_echo[0] == '$') {
            char *value = get_variable_value(to_echo + 1); // skip '$'
            if (value == NULL) {
                fprintf(stderr, "ERROR(hnd_echo): Bad variable name: %s\n", to_echo);
                return -1;
            }
            to_echo = value;
        }

        printf("%s", to_echo);
        if (i + 1 < argc) printf(" ");
    }
    printf("\n");
    return 0;
}
