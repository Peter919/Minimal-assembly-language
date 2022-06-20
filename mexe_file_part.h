// doesn't include imports, because those will be
// initialized within the symbol table by making
// elements point to labels

#pragma once
#include "log.h"

typedef unsigned char byte_t;

// the mexe representation of a masm file
struct MexeFilePart
{
        byte_t ** symbol_table;
        byte_t ** labels;
};

void log_instruction(enum e_LogLevel log_level, byte_t instruction);
