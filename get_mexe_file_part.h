#pragma once
#include "mexe_file_part.h"

unsigned long long read_int(char * mem_pos, unsigned int length);

void skip_imports(char ** buffer_pos);

struct MexeFilePart * get_mexe_file_part(char ** buffer_pos, unsigned int file_index);
