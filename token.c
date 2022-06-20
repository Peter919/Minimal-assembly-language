// tokens (aka minimal pieces of code - keywords, identifiers etc)
// some of them have values (like index for instruction and name
// for instructions), which are void pointers

#include "token.h"
#include <stdlib.h>
#include "settings.h"
#include "log.h"

static void log_list_element(enum e_LogLevel log_level, char * list[], unsigned char index)
{
        logger(log_level, list[index]);
}

void log_token(enum e_LogLevel log_level, void * token)
{
        // for efficiency
        if (log_level < g_min_log_level) {
                return;
        }

        struct Token * tokptr = (struct Token *) token;
        char * log_string;

        switch (tokptr->type) {
        case TOK_INVALID:
                log_string = "INVALID";
                break;
        case TOK_COMMENT:
                log_string = "COMMENT";
                break;
        case TOK_INSTRUCTION:
                log_string = "INSTRUCTION";
                break;
        case TOK_LABEL:
                log_string = "LABEL";
                break;
        case TOK_IDENTIFIER:
                log_string = "IDENTIFIER";
                break;
        case TOK_BUILTIN:
                log_string = "BUILT-IN";
                break;
        case TOK_NEWLINE:
                log_string = "NEWLINE";
                break;
        case TOK_WHITESPACE:
                log_string = "WHITESPACE";
                break;
        }

        logger(log_level, "%s", log_string);

        switch (tokptr->type) {
        case TOK_IDENTIFIER:
                // one of these might be used if its guaranteed that the token
                // is either a string or has been converted to an index
                // logger(log_level, " (%s)", (char *) tokptr->value);
                // logger(log_level, " (%u)", *(unsigned char *) tokptr->value);
                break;
        case TOK_BUILTIN:
                logger(log_level, " (");
                log_list_element(log_level, g_builtin_identifier_names, *(unsigned char *) tokptr->value);
                logger(log_level, ")");
                break;
        case TOK_INSTRUCTION:
                logger(log_level, " (");
                log_list_element(log_level, g_instruction_names, *(unsigned char *) tokptr->value);
                logger(log_level, ")");
                break;
        default:
                break;
        }
}

void free_token(void * token)
{
        struct Token * tokptr = (struct Token *) token;
        free(tokptr->value);
        free(tokptr);
}
