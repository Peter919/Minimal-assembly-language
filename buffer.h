#pragma once
#include "log.h"

struct Buffer
{
        char * contents;
        // signed to allow for error codes
        long size;
};

void log_buffer(enum e_LogLevel log_level, void * buffer);

void free_buffer(void * buffer);
