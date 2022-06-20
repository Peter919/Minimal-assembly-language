#pragma once
#include "log.h"

struct IdString {
        unsigned char id;
        char * str;
};

void free_id_string(void * id_string);

void log_id_string(enum e_LogLevel log_level, void * id_string);
