// getting the value of a token based on its type and moving to the next one

#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "token.h"
#include "token_recognition_functions.h"

void scan_comment(char ** scanning_pos)
{
        while (**scanning_pos != '\n' && **scanning_pos) {
                ++*scanning_pos;
        }
}

void scan_instruction(char ** scanning_pos, struct Token * token)
{
        unsigned char * instruction = malloc(sizeof(unsigned char));
        *instruction = word_strarr_index(g_instruction_names, *scanning_pos);

        token->value = (void *) instruction;

        while (is_identifier_symbol(**scanning_pos)) {
                ++*scanning_pos;
        }
}

void scan_builtin(char ** scanning_pos, struct Token * token)
{
        unsigned char * identifier_index = malloc(sizeof(unsigned char));
        *identifier_index = word_strarr_index(g_builtin_identifier_names, *scanning_pos);

        token->value = (void *) identifier_index;

        while (is_identifier_symbol(**scanning_pos)) {
                ++*scanning_pos;
        }
}

static unsigned int identifier_name_length(char * identifier)
{
        int length = 0;
        for (char * ch = identifier; is_identifier_symbol(*ch); ch++) {
                ++length;
        }
        return length;
}

void scan_identifier(char ** scanning_pos, struct Token * token)
{
        unsigned int idenlength = identifier_name_length(*scanning_pos);
        char * identifier_name = malloc(idenlength + 1);
        strncpy(identifier_name, *scanning_pos, idenlength);
        identifier_name[idenlength] = 0;

        token->value = (void *) identifier_name;
        *scanning_pos += idenlength;
}

void scan_newline(char ** scanning_pos, unsigned int * line)
{
        ++*scanning_pos;
        ++*line;
}

void scan_label(char ** scanning_pos)
{
        *scanning_pos += strlen(g_label_keyword);
}
