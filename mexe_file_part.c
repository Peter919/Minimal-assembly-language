// log_instruction is used solely for debugging purposes

#include <stdlib.h>
#include "mexe_file_part.h"
#include "log.h"
#include "settings.h"

void log_instruction(enum e_LogLevel log_level, byte_t instruction)
{
        if (log_level < g_min_log_level) {
                return;
        }

        char * instruction_name = g_instruction_names[(instruction & 0b11100000) >> 5];
        logger(log_level, "%s %d", instruction_name, instruction & 0b00011111);
}
