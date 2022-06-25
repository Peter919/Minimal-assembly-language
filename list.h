#pragma once
#include "log.h"

struct Node {
        void * value;
        struct Node * next;
};

struct List {
        struct Node * start;
        struct Node * end;
        unsigned int length;
};

void * list_index(struct List * list, unsigned int index);

void list_append(struct List * list, void * value);

char list_insert(struct List * list, unsigned int index, void * value);

char list_insert_after_node(struct Node * node, void * value);

char list_pop(struct List * list, unsigned int index, void (* free_value)(void *));

void list_combine(struct List * dest, struct List * src);

void list_delete(struct List * list, void (*free_value)(void *));

void log_list(enum e_LogLevel log_level, struct List * list, void (* node_print_function)(enum e_LogLevel, void *), char * separator);
