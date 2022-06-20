#pragma once
#include "list.h"

void write_int_to_buffer(char ** buffer_pos, unsigned char length, long long value);

void translate_labels(char ** buffer_pos, struct List * lexed_program);
