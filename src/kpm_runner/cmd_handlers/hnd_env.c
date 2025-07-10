
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef _WIN32
// Windows doesn't have setenv, so we implement it
int setenv(const char *name, const char *value, int overwrite) {
    if (!overwrite && getenv(name)) return 0;
    size_t len = strlen(name) + strlen(value) + 2;
    char *env_str = malloc(len);
    if (!env_str) return -1;
    snprintf(env_str, len, "%s=%s", name, value);
    return _putenv(env_str);
}

#include <windows.h>
#endif

int hnd_env(char **argv, size_t argc)
{
    if (argc == 1) {
        // print all env vars
        #ifdef _WIN32
            LPTCH env_strings = GetEnvironmentStrings();
            if (!env_strings) {
                fprintf(stderr, "Failed to get environment strings.\n");
                return 1;
            }

            LPTSTR var = (LPTSTR)env_strings;
            while (*var) {
                printf("%s\n", var);
                var += strlen(var) + 1;
            }

            FreeEnvironmentStrings(env_strings);
        #else
            extern char **environ;
            for (char **env = environ; *env != 0; env++)
                printf("%s\n", *env);
        #endif
    } else if (argc == 2) {
        // get var
        char *val = getenv(argv[1]);
        if (val) printf("%s=%s\n", argv[1], val);
        else fprintf(stderr, "env: variable '%s' not set\n", argv[1]);
    } else {
        // set var
        if (setenv(argv[1], argv[2], 1) != 0) {
            perror("env set");
            return 1;
        }
    }
    return 0;
}