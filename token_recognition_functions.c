// simple string functions that helps recognize the type
// of token it starts with

#include <string.h>
#include "settings.h"

char is_prefix(char * pre, char * str)
{
        char * pre_ch = pre;
        char * str_ch = str;

        while (*pre_ch) {
                if (*pre_ch != *str_ch) {
                        return 0;
                }

                ++pre_ch;
                ++str_ch;
        }
        return 1;
}

char is_identifier_symbol(char ch)
{
        // apparently, strchr(anything, 0) doesn't return 0
        if (ch == 0) {
                return 0;
        }

        if (strchr(g_identifier_primary_symbols, ch)) {
                return 2;
        } else if (strchr(g_identifier_secondary_symbols, ch)) {
                return 1;
        }
        return 0;
}

// ugliest function created, but at least it's short :)
// returns the index of strarr that equals the first "word"
// of str (aka all the characters of str that is_identifier_symbol)
int word_strarr_index(char * strarr[], char * str)
{
        unsigned int i = 0;
        for (;*strarr[i]; i++) {
                if (!is_prefix(strarr[i], str)) {
                        continue;
                }

                unsigned int word_length = strlen(strarr[i]);
                if (is_identifier_symbol(str[word_length])) {
                        continue;
                }

                return i;
        }
        return -1;
}
