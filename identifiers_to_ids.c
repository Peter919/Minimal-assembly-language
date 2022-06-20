// converts EVERY. SINGLE. IDENTIFIER. VALUE (hooray!)
// from identifier name to index in symbol table, which
// may be less than 16 (global) or more (local)

#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "list.h"
#include "masm_file.h"
#include "id_string.h"
#include "token.h"

#define MAX_GLOBALS 16

static int id_of_str_from_list(char * str, struct List * id_str_list)
{
        struct Node * node = id_str_list->start;
        struct IdString * id_str;
        while (node) {
                id_str = (struct IdString *) node->value;

                if (strcmp(str, id_str->str) == 0) {
                        return id_str->id;
                }

                node = node->next;
        }
        return -1;
}

static char token_pair_to_id(struct TokenPair * tokpair, struct List * locals, struct List * globals)
{
        if (tokpair->token->type != TOK_IDENTIFIER) {
                return 1;
        }
        if (tokpair->prev_tok == TOK_LABEL) {
                return 1;
        }

        unsigned char * id = malloc(sizeof(unsigned char));
        *id = 0;

        int index = id_of_str_from_list((char *) tokpair->token->value, globals);
        if (index != -1) {
                *id = (unsigned char) index;
                tokpair->token->value = (void *) id;
                return 1;
        }

        index = id_of_str_from_list((char *) tokpair->token->value, locals);
        if (index == -1) {
                free(id);
                return 0;
        }

        index += MAX_GLOBALS;
        *id = (unsigned char) index;
        tokpair->token->value = (void *) id;
        return 1;
}

char identifiers_to_ids(struct MasmFile * masm_file, struct List * globals)
{
        logger(LOG_INFO, "Converting the identifiers in \"%s\" to global and local IDs ...\n", masm_file->fname);
        struct List * locals = &masm_file->locals;

        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_INVALID;

        while (node) {
                tokpair.token = (struct Token *) node->value;

                if (!token_pair_to_id(&tokpair, locals, globals)) {
                        logger(LOG_ERROR, "Unknown identifier \"%s\" in \"%s\", line %u.\n",
                               (char *) tokpair.token->value, masm_file->fname, tokpair.token->line);
                        return 0;
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }

        logger(LOG_SUCCESS, "Done converting the identifiers in \"%s\" to global and local IDs ...\n", masm_file->fname);
        return 1;
}
