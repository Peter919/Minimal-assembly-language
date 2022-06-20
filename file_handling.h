#pragma once
#include "list.h"
#include "buffer.h"

char * get_dirpath(char * fpath);

char * get_filepath(char * dirpath, char * fname);

struct List files_in_dir(char * dirpath);

char * file_ext(char * fname);

char * file_to_string(char * fname);

struct Buffer file_to_buffer(char * fname);

char write_buffer_to_file(struct Buffer * buffer, char * fpath);
