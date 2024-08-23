#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void remove_trailing_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}