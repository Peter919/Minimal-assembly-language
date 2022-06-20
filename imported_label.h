#pragma once
#include "log.h"

struct ImportedLabel {
        unsigned int index_of_file; // guaranteed to be more than 0
        unsigned int index_of_label;
        unsigned char symbol_table_index;
};

void free_imported_label(void * imported_label);

void log_imported_label(enum e_LogLevel log_level, void * imported_label);
