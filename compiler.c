// mostly just calls functions from masm_file_list.c
// sets up the .mexe file of dirpath so it can be ran

#include "log.h"
#include "file_handling.h"
#include "settings.h"
#include "masm_file.h"
#include "masm_file_list.h"
#include "translate_masm_file_list.h"
#include "get_executable_file_path.h"

char compile(char * dirpath)
{
        struct List masm_file_list = init_masm_file_list(dirpath);

        if (!masm_file_list.length) {
                return 0;
        }
        newlines(LOG_DEBUG, 1);

        if (g_ignore_case) {
                lower_masm_file_list(&masm_file_list);
        }
        newlines(LOG_DEBUG, 1);

        if (!lex_masm_file_list(&masm_file_list)) {
                return 0;
        }
        newlines(LOG_DEBUG, 1);

        if (!parse_masm_file_list(&masm_file_list)) {
                return 0;
        }
        newlines(LOG_DEBUG, 1);


        struct Buffer mexe_buffer = translate_masm_file_list(&masm_file_list);
        char * exe_fpath = get_executable_file_path(dirpath);

        if (!write_buffer_to_file(&mexe_buffer, exe_fpath)) {
                return 0;
        }

        return 1;
}
