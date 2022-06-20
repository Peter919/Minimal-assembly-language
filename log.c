// includes functions that logs things with levels
// for instance, logger(LOG_ERROR, "An error occured.\n"); will print
// ERROR: An error occured.
// change min_log_level in settings to decide what's being logged and not

#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#include "settings.h"

// the only clean function i have
static void print_loglvl(enum e_LogLevel log_level)
{
        switch (log_level) {
        case LOG_DEBUG:
                printf("DEBUG: ");
                break;
        case LOG_INFO:
                printf("INFO: ");
                break;
        case LOG_SUCCESS:
                printf("SUCCESS: ");
                break;
        case LOG_WARNING:
                printf("WARNING: ");
        case LOG_ERROR:
                printf("ERROR: ");
                break;
        case LOG_FATAL_ERROR:
                printf("FATAL ERROR: ");
                break;
        default:
                break;
        }
}

// copied from
// https://stackoverflow.com/questions/68154231/how-do-i-define-a-function-that-accepts-a-formatted-input-string-in-c?rq=1
// from https://stackoverflow.com/users/3923896/steve-summit
// thanks, Steve!

char logger(enum e_LogLevel log_level, const char * text, ...)
{
        static int last_loglvl = -1;

        if(log_level < g_min_log_level) {
                return 0;
        }

        if (last_loglvl != log_level) {
                print_loglvl(log_level);
        }
        last_loglvl = log_level;

        va_list argp;
        va_start(argp, text);

        vprintf(text, argp);

        va_end(argp);

        return 1;
}

// prints newlines ... duh
char newlines(enum e_LogLevel log_level, unsigned int amount)
{
        if (log_level < g_min_log_level) {
                return 0;
        }

        for (int i = 0; i < amount; i++) {
                printf("\n");
        }

        return 1;
}
