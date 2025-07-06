#ifndef COMPILE_UTILS_H
#define COMPILE_UTILS_H
#include "stdlib.h"
char* parse_compile_command(const char* template,
                            const char** user_flags, size_t flag_count,
                            const char* output,
                            const char** input, size_t input_count);

#endif // COMPILE_UTILS_H
