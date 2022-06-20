#include <string.h>
#include "masm_file.h"

static char lower_char(char ch)
{
        static char * uppercase_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static char * lowercase_alphabet = "abcdefghijklmnopqrstuvwxyz";

        char * ch_in_uppercase = strchr(uppercase_alphabet, ch);
        if (!ch_in_uppercase) {
                return ch;
        }

        unsigned int index = (unsigned int) (ch_in_uppercase - uppercase_alphabet);

        return lowercase_alphabet[index];
}

char lower_masm_file(struct MasmFile * masm_file)
{
        logger(LOG_DEBUG, "Converting \"%s\" to lowercase ...\n", masm_file->fname);

        for (char * ch = masm_file->contents; *ch; ch++) {
                *ch = lower_char(*ch);
        }

        logger(LOG_SUCCESS, "Done converting \"%s\" to lowercase.\n", masm_file->fname);
        return 1;
}
