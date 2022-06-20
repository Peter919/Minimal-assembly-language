#pragma once
#include "masm_file.h"

void remove_token_type(struct List * lexed_file, enum e_Token token_type);

char set_label_list_to_label_names(struct MasmFile * masm_file);

char parse(struct MasmFile * masm_file, struct List * masm_file_list);

char remove_label_names(struct MasmFile * masm_file);
