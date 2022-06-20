// running the instructions happens in execute_main.c
// this is more of an overarching process thing
// that translates a mexe file and calls execute_main

#include "list.h"
#include "file_handling.h"
#include "mexe_file_part.h"
#include "get_executable_file_path.h"
#include "buffer_to_mexe_file_part_list.h"
#include "set_mexe_file_part_imports.h"
#include "initialize_symbol_tables.h"
#include "execute_main.h"

char run(char * dirpath)
{
        char * mexe_fpath = get_executable_file_path(dirpath);
        struct Buffer buffer = file_to_buffer(mexe_fpath);
        if (!buffer.contents) {
                return 0;
        }

        struct List mexe_file_parts = buffer_to_mexe_file_part_list(&buffer);
        set_imports(&buffer, &mexe_file_parts);

        byte_t ** global_symbol_table = initialize_global_symbol_table(&mexe_file_parts);
        byte_t ** other_symbol_tables = mexe_file_parts_to_symbol_tables(&mexe_file_parts);

        execute_main(global_symbol_table, other_symbol_tables);

        // it won't reach this place
        return 1;
}
