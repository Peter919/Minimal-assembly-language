// scans a buffer and converts it to a mexe file part
// must be done in 2 steps:
// 1. initialize all the symbol tables
// 2. add imports to the symbol tables

#include <stdlib.h>
#include <string.h>
#include "mexe_file_part.h"

#define SIZEOF_IMPORT_FILE_INDEX 2
#define SIZEOF_IMPORT_LABEL_INDEX 2
#define SIZEOF_IMPORT_SYMBOL_TABLE_INDEX 1
#define SIZEOF_IMPORT (SIZEOF_IMPORT_FILE_INDEX + SIZEOF_IMPORT_LABEL_INDEX + SIZEOF_IMPORT_SYMBOL_TABLE_INDEX)
#define SIZEOF_LABEL_LENGTH 2
#define SYMBOL_TABLE_LENGTH 16

#define FREE_IPT 0b01110001

unsigned long long read_int(char * mem_pos, unsigned int length)
{
        unsigned long long value = 0;
        char * curr_byte = mem_pos;

        unsigned char bitshift_amount = length * 8;
        while (bitshift_amount > 0) {
                bitshift_amount -= 8;

                value |= (*curr_byte & 0xff) << bitshift_amount;

                ++curr_byte;
        }

        return value;
}

// in case you just wanna read the labels
void skip_imports(char ** buffer_pos)
{
        // a file index of 0xffff terminates the imports
        while (read_int(*buffer_pos, SIZEOF_IMPORT_FILE_INDEX) != 0xffff) {
                *buffer_pos += SIZEOF_IMPORT;
        }
        // skips the 0xffff that terminates the imports
        *buffer_pos += SIZEOF_IMPORT_FILE_INDEX;
}

// useful for figuring out how much memory to malloc
// for pointers to labels
static unsigned short amount_of_labels(char * labels_start)
{
        unsigned short amount_of_labels = 0;

        char * buffer_pos = labels_start;

        unsigned short curr_label_length = read_int(buffer_pos, SIZEOF_LABEL_LENGTH);

        while (curr_label_length != 0xffff) {
                buffer_pos += SIZEOF_LABEL_LENGTH + curr_label_length;
                curr_label_length = read_int(buffer_pos, SIZEOF_LABEL_LENGTH);

                ++amount_of_labels;
        }
        return amount_of_labels;
}

static void write_int(long long value, byte_t * dest, unsigned char length)
{
        byte_t * curr_byte = dest;

        unsigned char bitshift_amount = length * 8;
        while (bitshift_amount > 0) {
                bitshift_amount -= 8;

                *curr_byte = (value >> bitshift_amount) & 0xff;
                ++curr_byte;
        }
}

static byte_t * get_label_contents(char ** label, unsigned int file_index)
{
        unsigned short label_length = read_int(*label, SIZEOF_LABEL_LENGTH);
        *label += SIZEOF_LABEL_LENGTH;

        byte_t * label_contents = malloc(sizeof(byte_t) + SIZEOF_IMPORT_FILE_INDEX + label_length);
        byte_t * writing_pos = label_contents;
        label_contents += 1 + SIZEOF_IMPORT_FILE_INDEX;

        // free ipt is a command that will make the program crash immediately
        // it's therefore used as a mark in labels to show that the symbol
        // table should change to the one with an index equal to the next
        // two bytes
        writing_pos[0] = FREE_IPT;
        writing_pos += sizeof(byte_t);

        // adds the index of the file (which will be converted to nothing
        // but a symbol table) to label_contents
        write_int(file_index, writing_pos, SIZEOF_IMPORT_FILE_INDEX);
        writing_pos += SIZEOF_IMPORT_FILE_INDEX;

        memcpy((void *) writing_pos, (void *) *label, label_length);
        *label += label_length;

        return label_contents;
}

// makes some elements of the symbol table in mexe_file_part point
// to the labels it has imported
static void set_labels(struct MexeFilePart * mexe_file_part, char ** labels_start, unsigned int file_index)
{
        unsigned short label_amount = amount_of_labels(*labels_start);
        mexe_file_part->labels = malloc(sizeof(byte_t *) * label_amount);

        for (unsigned short i = 0; i < label_amount; i++) {
                mexe_file_part->labels[i] = get_label_contents(labels_start, file_index);
        }
        *labels_start += SIZEOF_LABEL_LENGTH;
}

// initializes symbol table and labels
// imports must be done later, after the labels
// of all other files are initialized too
struct MexeFilePart * get_mexe_file_part(char ** buffer_pos, unsigned int file_index)
{
        skip_imports(buffer_pos);

        struct MexeFilePart * mexe_file_part = malloc(sizeof(struct MexeFilePart));
        set_labels(mexe_file_part, buffer_pos, file_index);
        mexe_file_part->symbol_table = malloc(sizeof(byte_t *) * SYMBOL_TABLE_LENGTH);

        return mexe_file_part;
}
