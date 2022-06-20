// translates the contents of a mexe file

#include "list.h"
#include "buffer.h"
#include "get_mexe_file_part.h"
#include "log.h"

struct List buffer_to_mexe_file_part_list(struct Buffer * buffer)
{
        logger(LOG_INFO, "Converting a buffer to a list of mexe file parts ...\n");

        struct List mexe_file_part_list = {0, 0, 0};
        struct MexeFilePart * mexe_file_part;

        unsigned int file_index = 0;

        char * buffer_pos = buffer->contents;
        while (buffer_pos != buffer->contents + buffer->size) {

                mexe_file_part = get_mexe_file_part(&buffer_pos, file_index);
                list_append(&mexe_file_part_list, (void *) mexe_file_part);
                ++file_index;
        }

        logger(LOG_SUCCESS, "Done converting a buffer to a list of mexe file parts.\n");
        return mexe_file_part_list;
}
