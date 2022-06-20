// makes elements in symbol tables point to the
// labels that are imported from the file

#include "list.h"
#include "buffer.h"
#include "mexe_file_part.h"
#include "imported_label.h"
// for read_int and skip_imports
#include "get_mexe_file_part.h"

#define SIZEOF_IMPORT_FILE_INDEX 2
#define SIZEOF_IMPORT_LABEL_INDEX 2
#define SIZEOF_IMPORT_SYMBOL_TABLE_INDEX 1
#define SIZEOF_IMPORT (SIZEOF_IMPORT_FILE_INDEX + SIZEOF_IMPORT_LABEL_INDEX + SIZEOF_IMPORT_SYMBOL_TABLE_INDEX)
#define SIZEOF_LABEL_LENGTH 2

static struct ImportedLabel read_import(char ** buffer_pos)
{
        struct ImportedLabel import = {0, 0, 0};

        import.index_of_file = read_int(*buffer_pos, SIZEOF_IMPORT_FILE_INDEX);
        *buffer_pos += SIZEOF_IMPORT_FILE_INDEX;

        import.index_of_label = read_int(*buffer_pos, SIZEOF_IMPORT_LABEL_INDEX);
        *buffer_pos += SIZEOF_IMPORT_LABEL_INDEX;

        import.symbol_table_index = read_int(*buffer_pos, SIZEOF_IMPORT_SYMBOL_TABLE_INDEX);
        *buffer_pos += SIZEOF_IMPORT_SYMBOL_TABLE_INDEX;

        return import;
}

static void set_file_part_imports(char * imports_start, struct MexeFilePart * mexe_file_part, struct List * mexe_file_part_list)
{
        struct ImportedLabel import;
        struct MexeFilePart * import_file;
        byte_t * label;

        char * buffer_pos = imports_start;
        while (read_int(buffer_pos, SIZEOF_IMPORT_FILE_INDEX) != 0xffff) {
                import = read_import(&buffer_pos);

                import_file = (struct MexeFilePart *) list_index(mexe_file_part_list, import.index_of_file);
                label = import_file->labels[import.index_of_label];
                mexe_file_part->symbol_table[import.symbol_table_index] = label;
        }
}

// we're only here for imports, so labels are skipped
static void skip_labels(char ** buffer_pos)
{
        unsigned int label_length = read_int(*buffer_pos, SIZEOF_LABEL_LENGTH);
        while (label_length != 0xffff) {
                *buffer_pos += SIZEOF_LABEL_LENGTH + label_length;
                label_length = read_int(*buffer_pos, SIZEOF_LABEL_LENGTH);
        }
        *buffer_pos += SIZEOF_LABEL_LENGTH;
}

void set_imports(struct Buffer * buffer, struct List * mexe_file_part_list)
{
        logger(LOG_INFO, "Initializing imports for a list of mexe file parts ...\n");

        unsigned long index = 0;
        struct MexeFilePart * mexe_file_part;

        char * buffer_pos = buffer->contents;
        while (buffer_pos != buffer->contents + buffer->size) {

                mexe_file_part = (struct MexeFilePart *) list_index(mexe_file_part_list, index);
                set_file_part_imports(buffer_pos, mexe_file_part, mexe_file_part_list);

                skip_imports(&buffer_pos);
                skip_labels(&buffer_pos);
                ++index;
        }

        logger(LOG_SUCCESS, "Done initializing imports for a list of mexe file parts.\n");
}
