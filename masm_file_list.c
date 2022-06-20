// kind of messy
// does all kinds of compilation stuff to a list of MasmFile-s
// from a directory

#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "settings.h"
#include "file_handling.h"
#include "masm_file.h"
#include "lower_masm_file.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "evaluate_imports.h"
#include "move_main_label_to_beginning.h"
#include "repeated_label_names.h"

// it could just as well be an int (or even a char), but i try to follow
// my own conventions, where indices in lists are unsigned ints, so
// the smallest type that includes any index and an error code is long
// lawyerd
static long get_masm_file(struct List * masm_file_list, char * masm_file_name)
{
        long index = 0;

        struct Node * node = masm_file_list->start;
        struct MasmFile * masm_file;
        while (node) {
                masm_file = (struct MasmFile *) node->value;

                if (strcmp(masm_file_name, masm_file->fname) == 0) {
                        return index;
                }

                node = node->next;
                ++index;
        }

        return -1;
}

// for list_pop without actually popping
void dont_free(void * value_not_to_free) {}

static char sort_masm_file_list(struct List * masm_file_list)
{
        logger(LOG_INFO, "Sorting the masm file list (global file, main file, all other files) ...\n");

        long main_file_index = get_masm_file(masm_file_list, g_main_file_name);
        if (main_file_index == -1) {
                logger(LOG_ERROR, "Couldn't find \"%s\".\n", g_main_file_name);
                return 0;
        }

        long global_file_index = get_masm_file(masm_file_list, g_global_file_name);
        if (global_file_index == -1) {
                logger(LOG_ERROR, "Couldn't find \"%s\".\n", g_global_file_name);
                return 0;
        }

        struct MasmFile * global_file = list_index(masm_file_list, global_file_index);
        struct MasmFile * main_file = list_index(masm_file_list, main_file_index);

        list_pop(masm_file_list, main_file_index, dont_free);
        if (main_file_index < global_file_index) {
                --global_file_index;
        }


        list_pop(masm_file_list, global_file_index, dont_free);
        list_insert(masm_file_list, 0, main_file);
        list_insert(masm_file_list, 0, global_file);

        logger(LOG_SUCCESS, "Done sorting the masm file list.\n");
        return 1;
}

// gets the files from dirpath and allocates MasmFiles
// for the files with the correct extension
struct List init_masm_file_list(char * dirpath)
{
        struct List masm_file_list = {0, 0, 0};

        struct List fname_list = files_in_dir(dirpath);

        struct Node * node = fname_list.start;
        // it's a for loop to make continue easier
        for (;node; node = node->next) {

                if (strcmp(file_ext((char *) node->value), g_masm_file_ext) != 0) {
                        continue;
                }

                struct MasmFile * masm_file = malloc(sizeof(struct MasmFile));
                // to make lists inside of masm_file have length and nodes of 0
                memset(masm_file, 0, sizeof(struct MasmFile));

                masm_file->fname = (char *) node->value;
                masm_file->fpath = get_filepath(dirpath, masm_file->fname);
                masm_file->contents = file_to_string(masm_file->fpath);

                if (!masm_file->fname || !masm_file->fpath || !masm_file->contents) {
                        free_masm_file((void *) masm_file);
                        logger(LOG_ERROR, "An error occurred during the initialization of the MASM file list.\n");
                        return (struct List) {0, 0, 0};
                }

                list_append(&masm_file_list, (void *) masm_file);
        }

        if (!sort_masm_file_list(&masm_file_list)) {
                return (struct List) {0, 0, 0};
        }

        return masm_file_list;
}

static char change_masm_file_list(struct List * masm_file_list, char (*masm_file_function)(struct MasmFile *))
{
        struct Node * node = masm_file_list->start;
        struct MasmFile * masm_file;
        while (node) {
                masm_file = (struct MasmFile *) node->value;

                if (!masm_file_function(masm_file)) {
                        return 0;
                }

                node = node->next;
        }
        return 1;
}

static char remove_whitespace(struct MasmFile * masm_file)
{
        remove_token_type(&masm_file->lexed, TOK_WHITESPACE);
        return 1;
}

// done to ignore case
void lower_masm_file_list(struct List * masm_file_list)
{
        change_masm_file_list(masm_file_list, lower_masm_file);
}

char lex_masm_file_list(struct List * masm_file_list)
{
        return change_masm_file_list(masm_file_list, lex);
}

char parse_masm_file_list(struct List * masm_file_list)
{
        logger(LOG_INFO, "Removing whitespace from all files and finding label names ...\n");

        change_masm_file_list(masm_file_list, remove_whitespace);
        if (!change_masm_file_list(masm_file_list, set_label_list_to_label_names)) {
                return 0;
        }

        struct MasmFile * global_file = (struct MasmFile *) masm_file_list->start->value;
        if (global_file->labels.length != 0) {
                logger(LOG_ERROR, "\"%s\" isn't supposed to have any labels.\n", global_file->fname);
                return 0;
        }

        logger(LOG_SUCCESS, "Done removing whitespace from all files and finding label names.\n");

        struct Node * node = masm_file_list->start;
        struct MasmFile * masm_file;
        while (node) {
                masm_file = (struct MasmFile *) node->value;

                if (!parse(masm_file, masm_file_list)) {
                        return 0;
                }

                node = node->next;
        }

        struct MasmFile * main_file;
        main_file = (struct MasmFile *) masm_file_list->start->next->value;
        if (!move_main_label_to_beginning(main_file)) {
                return 0;
        }

        if (global_file->imports.length != 0) {
                logger(LOG_ERROR, "Global variables can't be imports.\n");
                return 0;
        }
        if (multiple_labels_with_same_name(masm_file_list)) {
                return 0;
        }

        // globals.masm is no longer needed because it's guaranteed to have no
        // labels, and declarations are already removed
        list_pop(masm_file_list, 0, free_masm_file);

        change_masm_file_list(masm_file_list, remove_label_names);

        return 1;
}
