
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
int hnd_env(char **argv, size_t argc)
{
    if (argc == 1) {
        // print all env vars
#ifdef _WIN32
        // Windows: _environ is deprecated, but use for demo
        extern char **_environ;
        for (char **env = _environ; *env != 0; env++)
            printf("%s\n", *env);
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