#pragma once
#include "list.h"
#include "mexe_file_part.h"

byte_t ** initialize_global_symbol_table(struct List * mexe_file_parts);

byte_t ** mexe_file_parts_to_symbol_tables(struct List * mexe_file_parts);
