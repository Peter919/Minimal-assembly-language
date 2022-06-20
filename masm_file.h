#pragma once
#include "list.h"

struct MasmFile {
        char * fpath;
        char * fname;
        char * contents;
        struct List lexed;
        struct List labels;
        struct List imports;
        struct List locals;
};

void log_masm_file(enum e_LogLevel log_level, void * masm_file);

void free_masm_file(void * masm_file);
