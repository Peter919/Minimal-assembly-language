// tried to make it as efficient as possible, so it's REALLY ugly

#include <stdlib.h>
#include <time.h>
#include "settings.h"
#include "mexe_file_part.h"
#include "print_out.h"

#define SYMBOL_TABLE_LENGTH 16

#define GET 0
#define SET 1
#define ALLOCATE 2
#define FREE 3
#define ADDRESS 4
#define DEREFERENCE 5
#define NAND 6
#define ROTATE 7

#define VPT 0
#define IPT 1
#define IN 2
#define OUT 3
#define ONE 4

#define FREE_IPT 0b01100001
#define SET_IPT 0b00100001

static inline void _get(byte_t ** vpt, byte_t ** arg)
{
        **vpt = **arg;
}

static inline void _set(byte_t ** vpt, byte_t ** arg)
{
        *arg = *vpt;
}

static inline void _allocate(byte_t ** vpt, byte_t ** arg)
{
        *vpt = malloc(**arg);
}

static inline void _free(byte_t ** vpt, byte_t ** arg)
{
        free(*arg);
}

static inline void _address(byte_t ** vpt, byte_t ** arg)
{
        // first byte is used for offset
        **vpt = 0;

        byte_t * vpt_byte = *vpt + 1;
        byte_t * arg_adr_byte = (byte_t *) arg;

        // writes the address of *arg to the contents
        // of *vpt
        for (int i = 0; i < sizeof(byte_t **); i++) {
                vpt_byte[i] = arg_adr_byte[i];
        }
}

static inline void _dereference(byte_t ** vpt, byte_t ** arg)
{
        // gets the address that the first 8 bytes of
        // *arg is pointing to
        // first byte is used for offset
        *vpt = (byte_t *) (**arg + *(byte_t **) (*arg + 1));
}

static inline void _nand(byte_t ** vpt, byte_t ** arg)
{
        **vpt = ~(**vpt & **arg);
}

static inline void _rotate(byte_t ** vpt, byte_t ** arg)
{
        // a little messy to avoid extra time taking stuff
        // but it rotates bits to the right. there's no
        **vpt = (**vpt >> (**arg & 0b111)) | **vpt << (8 - (**arg & 0b111));
}

static void execute_command(byte_t ** global_symbol_table, byte_t ** symbol_tables, unsigned int symbol_table_index)
{
        for (;;) {
                print_out(global_symbol_table[OUT]);

                // instructions are first 3 bits of instructions,
                // then 1 bit that is 0 for global and 1 for local
                // then 4 bits for argument (index in symbol table)
                byte_t instr_and_arg = *global_symbol_table[IPT];
                ++global_symbol_table[IPT];

                log_instruction(LOG_FATAL_ERROR, instr_and_arg);
                newlines(LOG_FATAL_ERROR, 1);

                byte_t instr = (instr_and_arg & 0b11100000) >> 5;

                byte_t ** local_symbol_table;
                if (instr_and_arg & 0b00010000) {
                        // since local_symbol_table is a pointer, symbol_table_index * SYMBOL_TABLE_LENGTH
                        // is automatically multiplied by sizeof(byte *) so that's not needed
                        local_symbol_table = symbol_tables + symbol_table_index * SYMBOL_TABLE_LENGTH;
                } else {
                        local_symbol_table = global_symbol_table;
                }

                byte_t ** arg = &local_symbol_table[instr_and_arg & 0b00001111];
                byte_t ** vpt = &global_symbol_table[VPT];

                // 4 levels of indentation is not good, but
                // i'd have to call a function and dereference two pointers
                // to avoid it, so i sacrifice the cleanness for some speed
                if (instr_and_arg != SET_IPT) {
                        goto EXECUTE_INSTRUCTION;
                }

                // the first byte in the label is free ipt and the
                // two next bytes stores the index of the symbol table
                // that the label uses
                byte_t * label_beginning = *vpt - 2;
                while (*label_beginning != FREE_IPT) {
                        --label_beginning;
                }

                symbol_table_index = 0;
                symbol_table_index += label_beginning[1] * 0x100;
                symbol_table_index += label_beginning[2];
                global_symbol_table[IPT] = *vpt;
                continue;

                // should probably use function pointers,
                // but then there's a chance that the compiler
                // will ignore the fact that the functions are
                // inline, and the program will run slightly slower
        EXECUTE_INSTRUCTION:
                switch (instr) {
                case GET:
                        _get(vpt, arg);
                        break;
                case SET:
                        _set(vpt, arg);
                        break;
                case ALLOCATE:
                        _allocate(vpt, arg);
                        break;
                case FREE:
                        _free(vpt, arg);
                        break;
                case ADDRESS:
                        _address(vpt, arg);
                        break;
                case DEREFERENCE:
                        _dereference(vpt, arg);
                        break;
                case NAND:
                        _nand(vpt, arg);
                        break;
                case ROTATE:
                        _rotate(vpt, arg);
                        break;
                }
        }
}

void execute_main(byte_t ** global_symbol_table, byte_t ** other_symbol_tables)
{
        execute_command(global_symbol_table, other_symbol_tables, 0);
}
