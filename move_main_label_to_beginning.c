#include <string.h>
#include "log.h"
#include "list.h"
#include "settings.h"
#include "masm_file.h"
#include "token.h"

static struct Token * node_to_token(struct Node * node)
{
        return (struct Token *) node->value;
}

static char is_main_label(struct Node * node)
{
        if (node_to_token(node)->type != TOK_LABEL) {
                return 0;
        }
        if (strcmp((char *) node_to_token(node->next)->value, g_main_label_name) != 0) {
                return 0;
        }
        return 1;
}

static struct Node * node_before_main_label(struct MasmFile * main_file)
{
        struct Node * node = main_file->lexed.start;
        while (node->next->next) {
                if (is_main_label(node->next)) {
                        return node;
                }
                node = node->next;
        }
        return 0;
}

static struct Node * last_node_in_label(struct Node * start)
{
        struct Node * node = start;
        while (node->next) {
                if (node_to_token(node->next)->type == TOK_LABEL) {
                        return node;
                }
                node = node->next;
        }
        return node;
}

static void move_element_group_back(struct List * list, struct Node * node_before_start, struct Node * end)
{
        struct Node * start = node_before_start->next;

        if (!end->next) {
                list->end = node_before_start;
        }
        node_before_start->next = end->next;

        end->next = list->start;
        list->start = start;
}

char move_main_label_to_beginning(struct MasmFile * main_file)
{
        if (main_file->lexed.length == 0) {
                goto NO_MAIN_LABEL;
        }

        // checks if the main label is already in the beginning
        // of the main file
        if (is_main_label(main_file->lexed.start)) {
                return 1;
        }

        struct Node * node_before_main = node_before_main_label(main_file);
        if (!node_before_main) {
                goto NO_MAIN_LABEL;
        }

        struct Node * last_node_in_main = last_node_in_label(node_before_main->next);

        move_element_group_back(&main_file->lexed, node_before_main, last_node_in_main);

        return 1;

NO_MAIN_LABEL:
        logger(LOG_ERROR, "Couldn't find label \"%s\" in \"%s\".\n", g_main_label_name, g_main_file_name);
        return 0;
}
