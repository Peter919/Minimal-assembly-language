// translate one masm file to a buffer, which
// can later be combined with other buffers from
// the same masm project to create a mexe file

#include <stdlib.h>
#include "log.h"
#include "list.h"
#include "buffer.h"
#include "masm_file.h"
#include "token.h"
#include "imported_label.h"
#include "translate_labels.h"

static unsigned int length_of_translated_masm_file(struct MasmFile * masm_file)
{
        unsigned int length = 0;
        // 2 bytes for index of file, 2 bytes for index of label within file, 1 byte for symbol table index
        length += masm_file->imports.length * 5;
        // 2 bytes equal to 0xff to mark the end of imports
        length += 2;

        struct Node * node = masm_file->lexed.start;
        struct Token * token;
        while (node) {
                token = (struct Token *) node->value;

                if (token->type == TOK_LABEL) {
                        // 2 bytes to store label length
                        length += 2;
                } else if (token->type == TOK_INSTRUCTION) {
                        // each instruction takes 1 byte
                        ++length;
                }

                node = node->next;
        }

        return length;
}

static void translate_imports(char ** buffer_pos, struct List * imports)
{
        struct Node * node = imports->start;
        struct ImportedLabel * imported_label;
        while (node) {
                imported_label = (struct ImportedLabel *) node->value;

                write_int_to_buffer(buffer_pos, 2, imported_label->index_of_file);
                write_int_to_buffer(buffer_pos, 2, imported_label->index_of_label);
                write_int_to_buffer(buffer_pos, 1, imported_label->symbol_table_index);

                node = node->next;
        }
        write_int_to_buffer(buffer_pos, 2, 0xffff);
}

struct Buffer * translate_masm_file(struct MasmFile * masm_file)
{
        logger(LOG_INFO, "Translating \"%s\" ...\n", masm_file->fname);

        struct Buffer * buffer = malloc(sizeof(struct Buffer));
        buffer->size = length_of_translated_masm_file(masm_file);
        buffer->contents = malloc(buffer->size);

        char * buffer_pos = buffer->contents;
        translate_imports(&buffer_pos, &masm_file->imports);
        translate_labels(&buffer_pos, &masm_file->lexed);

        logger(LOG_INFO, "Done translating \"%s\".\n", masm_file->fname);

        return buffer;
}
