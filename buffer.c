#include "buffer.h"
#include <stdlib.h>
#include "log.h"
#include "settings.h"

void log_buffer(enum e_LogLevel log_level, void * buffer)
{
        if (log_level < g_min_log_level) {
                return;
        }

        struct Buffer * bufptr = (struct Buffer *) buffer;

        long curr_byte = 0;
        while (curr_byte < bufptr->size) {
                // printing it as a signed char would cause negative values
                // to be very long because they'd be treated as ints
                logger(log_level, "%02x", (unsigned char) bufptr->contents[curr_byte]);

                if (++curr_byte < bufptr->size) {
                        logger(log_level, " ");
                }
        }
}

void free_buffer(void * buffer)
{
        struct Buffer * bufptr = (struct Buffer *) buffer;
        free(bufptr->contents);
        free(bufptr);
}
