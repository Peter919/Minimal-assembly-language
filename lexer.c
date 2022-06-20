// converts a string into a list of tokens with values
// some of the things here can be changed through
// settings.c

#include <stdlib.h>
#include "settings.h"
#include "masm_file.h"
#include "token.h"
#include "token_recognition_functions.h"
#include "token_scanning_functions.h"

static char is_whitespace(char ch)
{
        switch (ch) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
                return 1;
        default:
                return 0;
        }
}

// this only gets the token type, not the value
static enum e_Token get_token(char * token)
{
        if (word_strarr_index(g_instruction_names, token) != -1) {
                return TOK_INSTRUCTION;
        }
        if (word_strarr_index(g_builtin_identifier_names, token) != -1) {
                return TOK_BUILTIN;
        }

        if (is_prefix("lab", token) && !is_identifier_symbol(token[3])) {
                return TOK_LABEL;
        }
        if (is_prefix("cmt", token) && !is_identifier_symbol(token[3])) {
                return TOK_COMMENT;
        }

        // 0 = not identifier, 1 = identifier but not first letter, 2 = can be first letter
        if (is_identifier_symbol(*token) == 2) {
                return TOK_IDENTIFIER;
        }
        if (*token == '\n') {
                return TOK_NEWLINE;
        }
        if (is_whitespace(*token)) {
                return TOK_WHITESPACE;
        }

        logger(LOG_ERROR, "Invalid symbol \'%c\'.\n", *token);
        return TOK_INVALID;
}

static struct Token * scan_token(char ** scanning_pos, unsigned int * line)
{
        struct Token * token = malloc(sizeof(struct Token));

        token->line = *line;
        token->type = get_token(*scanning_pos);
        token->value = 0;

        switch (token->type) {
        case TOK_INVALID:
                goto SCAN_TOKEN_ERROR;
        case TOK_COMMENT:
                scan_comment(scanning_pos);
                break;
        case TOK_INSTRUCTION:
                scan_instruction(scanning_pos, token);
                break;
        case TOK_LABEL:
                scan_label(scanning_pos);
                break;
        case TOK_IDENTIFIER:
                scan_identifier(scanning_pos, token);
                break;
        case TOK_BUILTIN:
                scan_builtin(scanning_pos, token);
                break;
        case TOK_NEWLINE:
                scan_newline(scanning_pos, line);
                break;
        default:
                ++*scanning_pos;
                break;
        }

        return token;
SCAN_TOKEN_ERROR:
        free_token(token);
        return 0;
}

char lex(struct MasmFile * masm_file)
{
        logger(LOG_INFO, "Lexically analysing \"%s\" ...\n", masm_file->fname);

        char * scanning_pos = masm_file->contents;
        unsigned int line = 1;

        struct Token * token;
        while (*scanning_pos) {
                token = scan_token(&scanning_pos, &line);

                if (!token) {
                        logger(LOG_ERROR, "An error occurred during the lexical analysis in line %u of \"%s\".\n", line, masm_file->fname);
                        return 0;
                }

                list_append(&masm_file->lexed, (void *) token);
        }

        logger(LOG_SUCCESS, "Done lexically analysing \"%s\".\n", masm_file->fname);
        return 1;
}
