#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
void remove_trailing_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
void lowercase(char *str) {
    while (*str) {
        *str = tolower((unsigned char) *str);
        str++;
    }
}

char *replace_string(const char *str, const char *old_substr, const char *new_substr) {
    char *result;
    int i, count = 0;
    int new_len = strlen(new_substr);
    int old_len = strlen(old_substr);

    // Counting the number of times the old word occurs in the string
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old_substr) == &str[i]) {
            count++;
            i += old_len - 1;
        }
    }

    // Allocating memory for the new result string
    result = (char *)malloc(i + count * (new_len - old_len) + 1);
    if (!result) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    i = 0;
    while (*str) {
        // Compare the substring with the result
        if (strstr(str, old_substr) == str) {
            strcpy(&result[i], new_substr);
            i += new_len;
            str += old_len;
        } else {
            result[i++] = *str++;
        }
    }

    result[i] = '\0';
    return result;
}