// includes basic settings that can be changed
// to avoid the need to rewrite code for small
// settings. big stuff like syntax can't really
// be changed here because it would take a lot
// of time to implement and won't be needed
// as far as i care

#include "log.h"

const int g_min_log_level = LOG_WARNING;
const char g_ignore_case = 1;
char * g_masm_file_ext = ".masm";
char * g_executable_file_ext = ".mexe";

char * g_main_label_name = "main";
char * g_main_file_name = "main.masm";
char * g_global_file_name = "globals.masm";

char * g_instruction_names[] = {"get", "set", "alc", "fre", "adr", "drf", "nnd", "rot", ""};
char * g_builtin_identifier_names[] = {"vpt", "ipt", "in", "out", "one", ""};
char * g_label_keyword = "lab";

// primary can be used at the beginning of the name
char * g_identifier_primary_symbols = "abcdefghijklmnnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";
char * g_identifier_secondary_symbols = "0123456789";

// the screen must be cleared between each call of print
// so it shouldn't be executed after every single instruction
const float g_seconds_between_printings = 1;
const float g_pause_seconds_after_print = 0;
