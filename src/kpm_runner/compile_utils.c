#include <stdio.h>
#include "../templates/custom.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include "compile_utils.h"
char* parse_compile_command(const char* template,
                            const char** user_flags, size_t flag_count,
                            const char* output,
                            const char** input, size_t input_count) {
    size_t buffer_size = 4096;
    char* result = (char*)malloc(buffer_size);
    if (!result) return NULL;
    result[0] = '\0';

    const char* p = template;
    while (*p) {
        if (strncmp(p, "{user_flags}", 12) == 0) {
            for (size_t i = 0; i < flag_count; ++i) {
                strncat(result, user_flags[i], buffer_size - strlen(result) - 1);
                if (i < flag_count - 1) {
                    strncat(result, " ", buffer_size - strlen(result) - 1);
                }
            }
            p += 12;
        } else if (strncmp(p, "{output}", 8) == 0) {
            strncat(result, output, buffer_size - strlen(result) - 1);
            p += 8;
        } else if (strncmp(p, "{input}", 7) == 0) {
            for (size_t i = 0; i < input_count; ++i) {
                strncat(result, input[i], buffer_size - strlen(result) - 1);
                if (i < input_count - 1) {
                    strncat(result, " ", buffer_size - strlen(result) - 1);
                }
            }
            p += 7;
        } else {
            strncat(result, (char[]){*p, '\0'}, buffer_size - strlen(result) - 1);
            p++;
        }
    }

    return result;
}

const char* json_type_to_string(json_type type) {
    switch (type) {
        case JSON_OBJECT: return "object";
        case JSON_ARRAY: return "array";
        case JSON_STRING: return "string";
        case JSON_INTEGER: return "integer";
        case JSON_REAL: return "real";
        case JSON_TRUE: return "true";
        case JSON_FALSE: return "false";
        case JSON_NULL: return "null";
        default: return "unknown";
    }
}
