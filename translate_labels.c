// translates labels from lexed form to .mexe form

#include "list.h"
#include "token.h"

void write_int_to_buffer(char ** buffer_pos, unsigned char length, long long value)
{
        unsigned char bitshift_amount = length * 8;

        while (bitshift_amount) {
                bitshift_amount -= 8;
                **buffer_pos = (value >> bitshift_amount) & 0xff;
                ++*buffer_pos;
        }
}

static char instruction_to_byte(struct Token * instruction, struct Token * argument)
{
        // instruction->value and argument->value are actually unsigned chars
        // but it really doesn't matter
        char instr_index = *(char *) instruction->value;
        char arg_index = *(char *) argument->value;
        return (instr_index << 5) | arg_index;
}

static struct Token * node_to_token(struct Node * node)
{
        return (struct Token *) node->value;
}

static void write_label_contents_to_buffer(char ** buffer_pos, struct Node ** label_start)
{
        struct Node * node = *label_start;
        node = node->next;
        while (node && node_to_token(node)->type != TOK_LABEL) {

                **buffer_pos = instruction_to_byte(node_to_token(node), node_to_token(node->next));
                ++*buffer_pos;

                // one for the instruction, one for the argument
                node = node->next->next;
        }
        *label_start = node;
}

// used to find the length of a label
static unsigned int instructions_until_next_label(struct Node * curr_label)
{
        unsigned int instr_count = 0;

        struct Node * node = curr_label->next;
        if (!node) {
                return 0;
        }

        struct Token * token;
        do {
                token = (struct Token *) node->value;

                if (token->type == TOK_INSTRUCTION) {
                        ++instr_count;
                }

                node = node->next;

        } while (node && token->type != TOK_LABEL);
        return instr_count;
}

void translate_labels(char ** buffer_pos, struct List * lexed_program)
{
        unsigned int label_length;

        struct Node * node = lexed_program->start;
        while (node) {
                label_length = instructions_until_next_label(node);
                write_int_to_buffer(buffer_pos, 2, label_length);
                write_label_contents_to_buffer(buffer_pos, &node);
        }
}
