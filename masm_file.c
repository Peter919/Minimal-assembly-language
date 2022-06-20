// basic functions for MasmFile, a struct containing
// all information needed for files with the .masm
// extension. most of the lexical analysis and parsing
// is spent changing a list of MasmFile-s

#include "masm_file.h"
#include <stdlib.h>
#include "log.h"
#include "token.h"
#include "imported_label.h"
#include "id_string.h"

void log_masm_file(enum e_LogLevel log_level, void * masm_file)
{
        struct MasmFile * mfptr = (struct MasmFile *) masm_file;
        logger(log_level, "%s (%s):\n", mfptr->fname, mfptr->fpath);
        logger(log_level, "%s\n", mfptr->contents);
        log_list(log_level, &mfptr->lexed, log_token, ", ");
}

void free_masm_file(void * masm_file)
{
        // a motherfucking pointer
        struct MasmFile * mfptr = (struct MasmFile *) masm_file;
        // this will free fname too because it's a pointer within fpath
        free(mfptr->fpath);
        free(mfptr->contents);
        list_delete(&mfptr->lexed, free_token);
        list_delete(&mfptr->imports, free_imported_label);
        list_delete(&mfptr->locals, free_id_string);
}
