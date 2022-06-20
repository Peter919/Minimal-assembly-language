// does some job in parsing a single file, although it needs to be set
// up before and after (see parse_masm_file_list in masm_file_list.c to
// see the details of what happens)

#include "list.h"
#include "masm_file.h"
#include "token.h"
#include "syntax_analysis.h"
#include "evaluate_imports.h"
#include "declarations_to_id_strings.h"
#include "identifiers_to_ids.h"

void remove_token_type(struct List * lexed_file, enum e_Token token_type)
{
        struct Node * node = lexed_file->start;
        unsigned int node_index = 0;

        struct Token * token;

        while (node) {
                token = (struct Token *) node->value;
                // changes node before it's possibly freed
                node = node->next;

                if (token->type == token_type) {
                        list_pop(lexed_file, node_index, free_token);
                } else {
                        ++node_index;
                }
        }
}

char set_label_list_to_label_names(struct MasmFile * masm_file)
{
        masm_file->labels = get_label_names(masm_file);

        if (masm_file->labels.start == (struct Node *) 0xDEAFFACE) {
                return 0;
        }

        return 1;
}

static char is_empty(struct List * lexed_file)
{
        struct Node * node = lexed_file->start;
        struct Token * token;
        while (node) {
                token = (struct Token *) node->value;

                // any non-empty file has either an identifier (declaration)
                // or label (or both)
                if (token->type == TOK_IDENTIFIER || token->type == TOK_LABEL) {
                        return 0;
                }

                node = node->next;
        }
        return 1;
}

char parse(struct MasmFile * masm_file, struct List * masm_file_list)
{
        logger(LOG_INFO, "Parsing \"%s\" ...\n", masm_file->fname);

        // why parse an empty file?
        if (is_empty(&masm_file->lexed)) {
                list_delete(&masm_file->lexed, free_token);
                logger(LOG_INFO, "\"%s\" was empty, so the parsing was skipped.\n", masm_file->fname);
                return 1;
        }

        if (!scan_syntax(masm_file)) {
                return 0;
        }

        remove_token_type(&masm_file->lexed, TOK_COMMENT);

        masm_file->labels = get_label_names(masm_file);

        if (!evaluate_imports(masm_file, masm_file_list)) {
                return 0;
        }

        // the global file is the first one in masm_file_list
        // this is so that when you check for identifier mismatches and stuff and have to
        // go through the list of global variables, you can check in global_file->locals
        // even if not all files have initialized their local variables yet
        // because global_file is guaranteed to be initialized first
        struct MasmFile * global_file = (struct MasmFile *) masm_file_list->start->value;
        struct List * global_identifiers = &global_file->locals; // globals->locals are actually just globals

        masm_file->locals = declarations_to_id_strings(masm_file, global_identifiers);
        if (masm_file->locals.start == (struct Node *) 0xDEAFFACE) {
                return 0;
        }

        remove_token_type(&masm_file->lexed, TOK_NEWLINE);
        delete_declarations(&masm_file->lexed);

        if (!identifiers_to_ids(masm_file, global_identifiers)) {
                return 0;
        }

        logger(LOG_SUCCESS, "Done parsing \"%s\".\n", masm_file->fname);
        return 1;
}

char remove_label_names(struct MasmFile * masm_file)
{
        struct Node * node = masm_file->lexed.start;
        unsigned int node_index = 0;

        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_INVALID;

        while (node) {
                tokpair.token = (struct Token *) node->value;
                // changes node before it's possibly freed
                node = node->next;

                if (tokpair.prev_tok == TOK_LABEL) {
                        list_pop(&masm_file->lexed, node_index, free_token);
                } else {
                        ++node_index;
                }

                tokpair.prev_tok = tokpair.token->type;
        }

        return 1;
}
