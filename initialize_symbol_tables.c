// creates enough space for local symbol tables
// and initializes all the builtins except for
// vpt (which doesn't need to be initialized)
// in the global symbol table

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "mexe_file_part.h"

#define MAX_INPUT_LENGTH 100000

#define SIZEOF_IMPORT_FILE_INDEX 2
#define SYMBOL_TABLE_LENGTH 16
#define SYMBOL_TABLE_BYTE_SIZE (SYMBOL_TABLE_LENGTH * sizeof(byte_t *))

#define IPT 1
#define IN 2
#define OUT 3
#define ONE 4

static char * get_input(void)
{
        char * input_buffer = malloc(MAX_INPUT_LENGTH);
        fgets(input_buffer, MAX_INPUT_LENGTH - 1, stdin);
        // removes newline from end of input
        input_buffer[strlen(input_buffer) - 1] = 0;

        char * input = malloc(strlen(input_buffer) + 1);
        strcpy(input, input_buffer);

        free(input_buffer);
        return input;
}

byte_t ** initialize_global_symbol_table(struct List * mexe_file_parts)
{
        byte_t ** global_symbol_table = malloc(SYMBOL_TABLE_BYTE_SIZE);

        global_symbol_table[IN] = (byte_t *) get_input();

        struct MexeFilePart * main_file;
        main_file = (struct MexeFilePart *) list_index(mexe_file_parts, 0);

        global_symbol_table[IPT] = main_file->labels[0];

        global_symbol_table[OUT] = malloc(1);
        *global_symbol_table[OUT] = 0;

        global_symbol_table[ONE] = malloc(1);
        *global_symbol_table[ONE] = 1;

        return global_symbol_table;
}

byte_t ** mexe_file_parts_to_symbol_tables(struct List * mexe_file_parts)
{
        byte_t ** symbol_tables = malloc(SYMBOL_TABLE_BYTE_SIZE * mexe_file_parts->length);
        byte_t ** curr_symbol_table = symbol_tables;

        struct Node * node = mexe_file_parts->start;
        struct MexeFilePart * mexe_file_part;
        while (node) {
                mexe_file_part = (struct MexeFilePart *) node->value;

                memcpy(curr_symbol_table, mexe_file_part->symbol_table, SYMBOL_TABLE_BYTE_SIZE);
                free(mexe_file_part->symbol_table);

                node = node->next;
                curr_symbol_table += SYMBOL_TABLE_LENGTH;
        }

        list_delete(mexe_file_parts, free);
        return symbol_tables;
}
