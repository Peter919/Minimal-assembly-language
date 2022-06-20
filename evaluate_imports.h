#pragma once
#include "list.h"
#include "masm_file.h"

struct List get_label_names(struct MasmFile * masm_file);

char evaluate_imports(struct MasmFile * masm_file, struct List * masm_file_list);
