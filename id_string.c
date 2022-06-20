// a string with an id, used for identifier stuff
// (identifier names and indices in the symbol table)

#include "id_string.h"
#include <stdlib.h>
#include "log.h"

void free_id_string(void * id_string)
{
        struct IdString * idstr_ptr = (struct IdString *) id_string;
        free(idstr_ptr->str);
        free(idstr_ptr);
}

void log_id_string(enum e_LogLevel log_level, void * id_string)
{
        struct IdString * idstr_ptr = (struct IdString *) id_string;
        logger(log_level, "%s: %u", idstr_ptr->str, idstr_ptr->id);
}
