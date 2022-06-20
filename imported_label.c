// information about imported label, which includes
// the index of the masm file in the masm file list,
// the index of the label within the masm file and
// the local symbol table index that has imported the
// label

#include "imported_label.h"
#include <stdlib.h>
#include "log.h"

void free_imported_label(void * imported_label)
{
        free(imported_label);
}

void log_imported_label(enum e_LogLevel log_level, void * imported_label)
{
        struct ImportedLabel * implab_ptr = (struct ImportedLabel *) imported_label;
        logger(log_level, "LOCAL SYMBOL TABLE INDEX %u: FILE %u, LABEL %u", implab_ptr->symbol_table_index, implab_ptr->index_of_file, implab_ptr->index_of_label);
}
