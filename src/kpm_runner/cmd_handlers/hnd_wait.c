#include <unistd.h> // Required for usleep()
#include <stdlib.h>
#include "stdio.h"
#if defined(WIN32)
#include <windows.h>
#endif
void hnd_linux_sleep(int milliseconds) {
    usleep(milliseconds * 1000); // Convert milliseconds to microseconds
}
#if defined(WIN32)
void hnd_windows_sleep(int milliseconds)
{
    
    Sleep(milliseconds);
    
}
#endif
int hnd_wait(char **argv, size_t argc)
{
    if(argc < 2)
    {
        fprintf(stderr,"ERROR(hnd_wait): Missing time specifier: wait <milliseconds>\n");
    }
    char *str_time_2_sleep = argv[1];
    int time_2_sleep = atoi(str_time_2_sleep);
    #if defined(WIN32)
    hnd_windows_sleep(time_2_sleep);
    #else
    hnd_linux_sleep(time_2_sleep);
    #endif
    return 0;
}