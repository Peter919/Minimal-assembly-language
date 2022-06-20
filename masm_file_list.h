#pragma once
#include "list.h"

struct List init_masm_file_list(char * dirpath);

void lower_masm_file_list(struct List * masm_file_list);

char lex_masm_file_list(struct List * masm_file_list);

char parse_masm_file_list(struct List * masm_file_list);
