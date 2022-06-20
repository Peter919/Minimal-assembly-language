// creates a list of struct IdString-s filled with the
// names and indices (in the symbol table) of identifiers

#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "settings.h"
#include "masm_file.h"
#include "token.h"
#include "id_string.h"

#define MAX_LOCALS 16

static struct IdString * create_id_string(unsigned char id, char * str)
{
        struct IdString * id_string = malloc(sizeof(struct IdString));
        id_string->id = id;
        id_string->str = malloc(strlen(str));
        strcpy(id_string->str, str);
        return id_string;
}

static struct IdString * token_to_id_string(struct TokenPair * tokpair, unsigned char * id)
{
        switch (tokpair->token->type) {
        case TOK_IDENTIFIER:
                return create_id_string(*id, (char *) tokpair->token->value);
        case TOK_NEWLINE:
                if (tokpair->prev_tok == TOK_IDENTIFIER) {
                        ++*id;
                }
                return 0;
        default:
                return 0;
        }
}

// checks if iden_name is redeclared either in locals or globals
static char is_identifier_redeclared(char * iden_name, struct List * locals, struct List * globals)
{
        struct Node * node = locals->start;
        struct IdString * id_string;
        while (node) {
                id_string = (struct IdString *) node->value;

                if (strcmp(iden_name, id_string->str) == 0 && iden_name != id_string->str) {
                        return 1;
                }

                node = node->next;
        }

        node = globals->start;
        while (node) {
                id_string = (struct IdString *) node->value;

                if (strcmp(iden_name, id_string->str) == 0 && iden_name != id_string->str) {
                        return 1;
                }

                node = node->next;
        }
        return 0;
}

static char * get_redeclaration(struct List * locals, struct List * globals)
{
        struct Node * node = locals->start;
        struct IdString * id_string;
        while (node) {

                id_string = (struct IdString *) node->value;

                if (is_identifier_redeclared(id_string->str, locals, globals)) {
                        return id_string->str;
                }

                node = node->next;
        }
        return 0;
}

static int amount_of_builtins(void)
{
        int i = 0;
        while (*g_builtin_identifier_names[i]) {
                ++i;
        }
        return i;
}

struct List declarations_to_id_strings(struct MasmFile * masm_file, struct List * globals)
{
        struct List id_string_list = {0, 0, 0};

        unsigned char id;
        if (strcmp(masm_file->fname, g_global_file_name) == 0) {
                // builtins fill up the first elements of globals
                id = amount_of_builtins();
        } else {
                id = 0;
        }

        struct IdString * id_string;

        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.token = (struct Token *) node->value;
        tokpair.prev_tok = TOK_INVALID;

        while (node && tokpair.token->type != TOK_LABEL) {
                tokpair.token = (struct Token *) node->value;

                id_string = token_to_id_string(&tokpair, &id);
                if (id_string) {
                        list_append(&id_string_list, (void *) id_string);
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }
        // id was wrongly increased one final time because the
        // last token was the last identifier declaration and
        // the current token was a newline
        if (id != 0) {
                --id;
        }

        // this really doesn't need to be checked for every iteration of the loop
        // max locals is guaranteed to equal max globals so it doesn't matter if globals.masm
        // is being checked

        if (id >= MAX_LOCALS) {
                logger(LOG_ERROR, "Too many identifier declarations in \"%s\".\n", masm_file->fname);
                return (struct List) {(struct Node *) 0xDEAFFACE, 0, 0};
        }

        char * redeclaration = get_redeclaration(&id_string_list, globals);
        if (redeclaration) {
                logger(LOG_ERROR, "\"%s\" has multiple declarations in \"%s\" or "
                       "shares the name of a global variable.\n", redeclaration, masm_file->fname);
                return (struct List) {(struct Node *) 0xDEAFFACE, 0, 0};
        }

        return id_string_list;
}

void delete_declarations(struct List * lexed_file)
{
        struct Node * node = lexed_file->start;
        struct Token * token = (struct Token *) node->value;

        while (token->type != TOK_LABEL) {
                list_pop(lexed_file, 0, free_token);

                node = lexed_file->start;
                if (!node) {
                        break;
                }

                token = (struct Token *) node->value;
        }
}
