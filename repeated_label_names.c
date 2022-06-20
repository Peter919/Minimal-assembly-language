#include <string.h>
#include "log.h"
#include "list.h"
#include "masm_file.h"
#include "token.h"

static char is_label_repeated_in_file(char * label_name, struct MasmFile * masm_file)
{
        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_INVALID;
        while (node) {
                tokpair.token = (struct Token *) node->value;

                if (tokpair.prev_tok != TOK_LABEL) {
                        goto CHECK_NEXT_TOKEN;
                }
                if (strcmp(label_name, (char *) tokpair.token->value) != 0) {
                        goto CHECK_NEXT_TOKEN;
                }
                // this means were comparing the label to itself, in
                // which case it's not a copy
                if (label_name == (char *) tokpair.token->value) {
                        goto CHECK_NEXT_TOKEN;
                }

                return 1;

                CHECK_NEXT_TOKEN:
                        node = node->next;
                        tokpair.prev_tok = tokpair.token->type;
        }
        return 0;
}

static char is_label_repeated(struct TokenPair * label, struct List * masm_file_list)
{
        if (label->prev_tok != TOK_LABEL) {
                return 0;
        }
        char * label_name = (char *) label->token->value;

        struct Node * node = masm_file_list->start;
        struct MasmFile * masm_file;
        while (node) {
                masm_file =  (struct MasmFile *) node->value;

                if (is_label_repeated_in_file(label_name, masm_file)) {
                        logger(LOG_ERROR, "Label \"%s\" redeclared in \"%s\".\n", label_name, masm_file->fname);
                        return 1;
                }

                node = node->next;
        }
        return 0;
}

static char repeated_labels_in_file(struct MasmFile * masm_file, struct List * masm_file_list)
{
        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_INVALID;
        while (node) {
                tokpair.token = (struct Token *) node->value;

                if (is_label_repeated(&tokpair, masm_file_list)) {
                        // it has, at this point, already given an error, but it does
                        // not include the name of masm_file
                        logger(LOG_ERROR, "Label \"%s\" from \"%s\" has multiple declarations.\n",
                               (char *) tokpair.token->value, masm_file->fname);
                        return 1;
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }
        return 0;
}

char multiple_labels_with_same_name(struct List * masm_file_list)
{
        struct Node * node = masm_file_list->start;
        struct MasmFile * masm_file;
        while (node) {
                masm_file = (struct MasmFile *) node->value;

                if (repeated_labels_in_file(masm_file, masm_file_list)) {
                        return 1;
                }

                node = node->next;
        }
        return 0;
}
