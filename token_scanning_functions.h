#pragma once

void scan_comment(char ** scanning_pos);

void scan_instruction(char ** scanning_pos, struct Token * token);

void scan_builtin(char ** scanning_pos, struct Token * token);

void scan_identifier(char ** scanning_pos, struct Token * token);

void scan_newline(char ** scanning_pos, unsigned int * line);

void scan_label(char ** scanning_pos);
