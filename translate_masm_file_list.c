// converts a masm file list to a mexe buffer

#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "list.h"
#include "buffer.h"
#include "translate_masm_file.h"

long combined_buffer_size(struct List * list_of_buffers)
{
        long total_size = 0;

        struct Node * node = list_of_buffers->start;
        struct Buffer * buffer;

        while (node) {
                buffer = (struct Buffer *) node->value;

                total_size += buffer->size;

                node = node->next;
        }

        return total_size;
}

void combine_masm_file_buffers(struct List * buffer_list, struct Buffer * dest)
{
        char * buffer_pos = dest->contents;

        struct Node * node = buffer_list->start;
        struct Buffer * buffer;
        while (node) {
                buffer = (struct Buffer *) node->value;

                memcpy(buffer_pos, buffer->contents, buffer->size);
                buffer_pos += buffer->size;
                // terminates masm file
                *(buffer_pos++) = 0xff;
                *(buffer_pos++) = 0xff;

                node = node->next;
        }
}

struct Buffer translate_masm_file_list(struct List * masm_file_list)
{
        logger(LOG_INFO, "Translating a MASM file list to a buffer ...\n");
        struct List buffer_list = {0, 0, 0};

        struct Node * node = masm_file_list->start;
        struct MasmFile * masm_file;
        while (node) {
                masm_file = (struct MasmFile *) node->value;

                list_append(&buffer_list, (void *) translate_masm_file(masm_file));

                node = node->next;
        }

        struct Buffer buffer;
        buffer.size = combined_buffer_size(&buffer_list);

        // each masm file is terminated by two bytes equal to 0xff
        buffer.size += buffer_list.length * 2;
        buffer.contents = malloc(buffer.size);

        combine_masm_file_buffers(&buffer_list, &buffer);
        list_delete(&buffer_list, free_buffer);
        list_delete(masm_file_list, free_masm_file);

        logger(LOG_SUCCESS, "Done translating a MASM file list to a buffer ...\n");
        return buffer;
}
