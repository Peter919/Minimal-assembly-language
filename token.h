#pragma once
#include "log.h"

enum e_Token {
        TOK_INVALID,
        TOK_COMMENT,

        TOK_INSTRUCTION,

        TOK_LABEL,
        // value = name
        TOK_IDENTIFIER,
        // value = index
        TOK_BUILTIN,

        TOK_NEWLINE,
        TOK_WHITESPACE
};

struct Token {
        unsigned int line;
        enum e_Token type;
        void * value;
};

// used to check syntax
struct TokenPair {
        struct Token * token;
        enum e_Token prev_tok;
};

void log_token(enum e_LogLevel log_level, void * token);

void free_token(void * token);
