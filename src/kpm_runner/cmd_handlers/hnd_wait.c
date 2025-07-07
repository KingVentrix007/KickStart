#include <unistd.h> // Required for usleep()
#include <stdlib.h>
void hnd_linux_sleep(int milliseconds) {
    usleep(milliseconds * 1000); // Convert milliseconds to microseconds
}

void hnd_windows_sleep(int milliseconds)
{
    
}

int hnd_wait(char **argv, size_t argc)
{
    char *str_time_2_sleep = argv[1];
    int time_2_sleep = atoi(str_time_2_sleep);
    #if defined(WIN32)
    hnd_windows_sleep(time_2_sleep);
    #else
    hnd_linux_sleep(time_2_sleep);
    #endif
    return 0;
}