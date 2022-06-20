// does some checking on whether tokens have the correct order

#include "masm_file.h"
#include "token.h"

static char scan_comment_syntax(struct MasmFile * masm_file)
{
        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_NEWLINE; // so files can start with a comment
        while (node) {
                tokpair.token = (struct Token *) node->value;

                if (tokpair.token->type == TOK_COMMENT && tokpair.prev_tok != TOK_NEWLINE) {
                        logger(LOG_ERROR, "Expected newline before ");
                        log_token(LOG_ERROR, tokpair.token);
                        logger(LOG_ERROR, " in \"%s\", line %u.\n", masm_file->fname, tokpair.token->line);
                        return 0;
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }
        return 1;
}

// tokes that are valid before the first label
static char valid_declaration_token(enum e_Token token)
{
        // whitespace is valid too, but already removed
        return token == TOK_IDENTIFIER || token == TOK_NEWLINE || token == TOK_COMMENT;
}

// local declarations are written before the first LABEL token
static char scan_declaration_syntax(struct MasmFile * masm_file)
{
        struct Node * node = masm_file->lexed.start;
        struct Token * token = (struct Token *) node->value;
        // to make beginning of file comments allowed

        while (token->type != TOK_LABEL) {
                if (!valid_declaration_token(token->type)) {
                        logger(LOG_ERROR, "Expected label before ");
                        log_token(LOG_ERROR, token);
                        logger(LOG_ERROR, " in \"%s\", line %u.\n", masm_file->fname, token->line);
                        return 0;
                }

                node = node->next;
                if (!node) {
                        break;
                }

                token = (struct Token *) node->value;
        }

        return 1;
}

static char goto_first_label(struct Node ** node, struct TokenPair * tokpair)
{
        while (tokpair->token->type != TOK_LABEL) {
                *node = (*node)->next;
                if (!*node) {
                        return 0;
                }

                tokpair->prev_tok = tokpair->token->type;
                tokpair->token = (struct Token *) (*node)->value;
        }

        return 1;
}

static char requires_arg(enum e_Token token)
{
        return token == TOK_LABEL || token == TOK_INSTRUCTION;
}

static char is_valid_syntax(struct TokenPair * tokpair)
{
        switch (tokpair->token->type) {
        case TOK_INSTRUCTION:
        case TOK_LABEL:
                if (tokpair->prev_tok == TOK_NEWLINE) {
                        return 1;
                }
                return 0;
        case TOK_IDENTIFIER:
                if (tokpair->prev_tok == TOK_LABEL) {
                        return 1;
                }
        case TOK_BUILTIN:
                if (tokpair->prev_tok == TOK_INSTRUCTION) {
                        return 1;
                }
                return 0;
        default:
                if (!requires_arg(tokpair->prev_tok)) {
                        return 1;
                }
                return 0;
        }
}

// after whitespace is removed, there must be a newline before every instruction, label and comment
static char scan_label_content_syntax(struct MasmFile * masm_file)
{
        struct Node * node = masm_file->lexed.start;
        struct TokenPair tokpair;
        tokpair.prev_tok = TOK_NEWLINE;
        tokpair.token = (struct Token *) node->value;

        if (!goto_first_label(&node, &tokpair)) {
                return 1;
        }

        while (node) {
                tokpair.token = (struct Token *) node->value;

                if (!is_valid_syntax(&tokpair)) {
                        logger(LOG_ERROR, "Unexpected ");
                        log_token(LOG_ERROR, tokpair.token);
                        logger(LOG_ERROR, " in \"%s\", line %u.\n", masm_file->fname, tokpair.token->line);
                        return 0;
                }

                node = node->next;
                tokpair.prev_tok = tokpair.token->type;
        }

        if (requires_arg(tokpair.token->type)) {
                logger(LOG_ERROR, "Expected argument before end of file in \"%s\", line %u.\n", masm_file->fname, tokpair.token->line);
                return 0;
        }

        return 1;
}

char scan_syntax(struct MasmFile * masm_file)
{
        if (!scan_comment_syntax(masm_file)) {
                return 0;
        }
        if (!scan_declaration_syntax(masm_file)) {
                return 0;
        }
        if (!scan_label_content_syntax(masm_file)) {
                return 0;
        }

        return 1;
}
