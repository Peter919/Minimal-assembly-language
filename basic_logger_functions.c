// includes basic logger functions
// needed for log_list

#include "log.h"

void log_string(enum e_LogLevel log_level, void * str)
{
        logger(log_level, "%s", (char *) str);
}
