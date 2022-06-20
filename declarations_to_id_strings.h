#pragma once
#include "list.h"

struct List declarations_to_id_strings(struct MasmFile * masm_file, struct List * globals);

void delete_declarations(struct List * lexed_file);
