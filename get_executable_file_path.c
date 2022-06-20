#include <stdlib.h>
#include <string.h>
#include "settings.h"

// includes the backslash (or slash)
static char * get_dirname(char * dirpath)
{
        char * last_backslash = strrchr(dirpath, '\\');
        return last_backslash ? last_backslash : strrchr(dirpath, '/');
}

char * get_executable_file_path(char * dirpath)
{
        char * dirname = get_dirname(dirpath);
        char * exe_fname = malloc(strlen(dirpath) + strlen(dirname) + strlen(g_executable_file_ext) + 1);

        strcpy(exe_fname, dirpath);
        strcat(exe_fname, dirname);
        strcat(exe_fname, g_executable_file_ext);

        return exe_fname;
}
