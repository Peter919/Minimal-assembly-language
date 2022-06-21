// i use evaluate whenever i don't know what word fits
// this thing creates a list of struct ImportedLabel-s
// from the labels a file imports from another one

#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "list.h"
#include "masm_file.h"
#include "token.h"
#include "imported_label.h"

#define MAX_LOCALS 16

static char * create_string_clone(char * str)
{
        char * clone = malloc(strlen(str) + 1);
        strcpy(clone, str);
        return clone;
}

static char move_main_label_back(struct List * label_names)
{
        unsigned int index = 0;
        struct Node * node = label_names->start;
        while (node) {
                if (strcmp((char *) node->value, g_main_label_name) == 0) {
                        list_pop(label_names, index, free);
                        list_insert(label_names, 0, create_string_clone(g_main_label_name));
                        return 1;
                }

                node = node->next;
                ++index;
        }
        logger(LOG_ERROR, "Couldn't find label \"%s\" in main file.\n", g_main_label_name);
        return 0;
}

struct List get_label_names(struct MasmFile * masm_file)
{
        struct List label_list = {0, 0, 0};

        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_INVALID; // value doesn't matter yet

        while (node) {
                tokpair.token = (struct Token *) node->value;

                if (tokpair.prev_tok == TOK_LABEL) {
                        list_append(&label_list, create_string_clone((char *) tokpair.token->value));
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }

        if (strcmp(masm_file->fname, g_main_file_name) == 0) {
                if (!move_main_label_back(&label_list)) {
                        return (struct List) {0, (struct Node *) 0xDEAFFACE, 0};
                }
        }

        return label_list;
}

// finds the index of label_name within label_list
static long label_list_index(char * label_name, struct List * label_list)
{
        struct Node * node = label_list->start;
        long index = 0;
        while (node) {
                if (strcmp(label_name, (char *) node->value) == 0) {
                        return index;
                }

                node = node->next;
                ++index;
        }
        return -1;
}

// allocates space for an imported_label and sets all of its values to what
// they should be based on the arguments
static struct ImportedLabel * get_import(struct Token * token, struct List * masm_file_list, unsigned int symbol_table_index)
{
        if (token->type != TOK_IDENTIFIER) {
                return 0;
        }

        struct ImportedLabel * imported_label;
        imported_label = malloc(sizeof(struct ImportedLabel));
        imported_label->symbol_table_index = symbol_table_index;

        unsigned int file_index = 0;
        long label_index = 0; // it's a long to allow for -1 as an error code

        struct Node * node = masm_file_list->start->next;
        struct MasmFile * masm_file;
        while (node) {
                masm_file = (struct MasmFile *) node->value;

                label_index = label_list_index((char *) token->value, &masm_file->labels);
                if (label_index != -1) {
                        imported_label->index_of_file = file_index;
                        imported_label->index_of_label = label_index;
                        return imported_label;
                }

                node = node->next;
                ++file_index;
        }
        return 0;
}

static char error_in_imports(struct MasmFile * masm_file)
{
        // each elemtent, i, will have its value assigned to 0 or
        // 1 depending on whether element i is a symbol table index
        // from masm_file->imports. if it finds a symbol table index
        // that is already assigned to 1 here, that's an error
        char imports[MAX_LOCALS];
        memset(imports, 0, MAX_LOCALS);

        struct Node * node = masm_file->imports.start;
        struct ImportedLabel * imp_lab;
        while (node) {
                imp_lab = (struct ImportedLabel *) node->value;

                if (imports[imp_lab->symbol_table_index]) {
                        logger(LOG_ERROR, "Multiple imported labels in \"%s\" "
                               "shares the same index in the symbol table.\n", masm_file->fname);
                        return 1;
                }
                imports[imp_lab->symbol_table_index] = 1;

                node = node->next;
        }
        return 0;
}

char evaluate_imports(struct MasmFile * masm_file, struct List * masm_file_list)
{
        struct ImportedLabel * imported_label;
        int symbol_table_index = -1;

        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.token = (struct Token *) node->value;
        tokpair.prev_tok = TOK_NEWLINE;

        while (tokpair.token->type != TOK_LABEL && node) {
                tokpair.token = (struct Token *) node->value;

                // every line with an identifier declaration has its own index in the local symbol table
                if (tokpair.token->type == TOK_IDENTIFIER && tokpair.prev_tok == TOK_NEWLINE) {
                        ++symbol_table_index;
                }

                imported_label = get_import(tokpair.token, masm_file_list, symbol_table_index);
                if (imported_label) {
                        list_append(&masm_file->imports, (void *) imported_label);
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }

        if (error_in_imports(masm_file)) {
                return 0;
        }
        return 1;
}
