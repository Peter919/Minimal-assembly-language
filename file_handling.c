// gets the content of files, directories etc
// the kind of file that doesn't necessarily
// need to be specific to this project

#include "file_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "log.h"
#include "basic_logger_functions.h"
#include "list.h"
#include "os.h"
#include "buffer.h"

char * get_dirpath(char * fpath)
{
        logger(LOG_INFO, "Getting the path to the directory containing \"%s\" ...\n", fpath);

        char * fname_begin = strrchr(fpath, '\\');
        if (!fname_begin) {
                fname_begin = strrchr(fpath, '/');
        }
        if (!fname_begin) {
                logger(LOG_ERROR, "\"%s\" was supposed to be a file path, but there was no slashes or backslashes in the string.\n", fpath);
                return 0;
        }

        // temporary early termination so new fpath = old fpath's dirpath
        char old_fname_begin = *fname_begin;
        *fname_begin = 0;

        char * dirpath = malloc(strlen(fpath) + 1);
        strcpy(dirpath, fpath);

        *fname_begin = old_fname_begin;
        logger(LOG_INFO, "Done getting the path to the directory containing \"%s\".\n", fpath);
        return dirpath;
}

char * get_filepath(char * dirpath, char * fname)
{
        char * fpath = malloc(strlen(dirpath) + strlen(fname) + 2);

        strcpy(fpath, dirpath);

        if (strchr(dirpath, '\\')) {
                strcat(fpath, "\\");
        } else if (strchr(dirpath, '/')) {
                strcat(fpath, "/");
        } else {
                logger(LOG_ERROR, "\"%s\" is supposed to be directory path, but no slashes or backslashes were found.\n");
                return 0;
        }

        strcat(fpath, fname);

        return fpath;
}

// from https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
// platform dependent
struct List files_in_dir(char * dirpath)
{
        logger(LOG_INFO, "Getting files from directory \"%s\" ...\n", dirpath);

        DIR * d = opendir(dirpath);
        struct dirent * dir;

        struct List fnamelist = {0, 0, 0};
        char * curr_fname;

        if (!d) {
                logger(LOG_ERROR, "Couldn't open directory \"%s\".\n", dirpath);
        }

        while ((dir = readdir(d)) != 0) {
                curr_fname = malloc(strlen(dir->d_name) + 1);
                strcpy(curr_fname, dir->d_name);
                list_append(&fnamelist, (void *) curr_fname);
        }
        closedir(d);

        // the first two elements are "." and ".."
        list_pop(&fnamelist, 0, free);
        list_pop(&fnamelist, 0, free);

        logger(LOG_DEBUG, "Files in \"%s\": ", dirpath);
        log_list(LOG_DEBUG, &fnamelist, log_string, ", ");
        newlines(LOG_DEBUG, 1);

        logger(LOG_SUCCESS, "Done getting files from directory \"%s\".\n", dirpath);
        newlines(LOG_SUCCESS, 1);
        return fnamelist;
}

char * file_ext(char * fname)
{
        char * ext = strrchr(fname, '.');
        if (!ext) {
                logger(LOG_ERROR, "Couldn't find the file extension of \"%s\".\n", fname);
                return ext;
        }
        return ext;
}

static unsigned int chars_in_string(char * str, char ch, unsigned int length)
{
        unsigned int count = 0;

        char * curr_ch = str;
        for (int i = 0; i < length; i++) {
                if (*curr_ch == ch) {
                        ++count;
                }
                ++curr_ch;
        }
        return count;
}

static long file_length(FILE * fp)
{
        fseek(fp, 0, SEEK_END);
        unsigned long flength = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        return flength;
}

// converts file to null-terminated string
char * file_to_string(char * fname)
{
        logger(LOG_INFO, "Moving the contents of \"%s\" to a string ...\n", fname);

        FILE * fp = fopen(fname, "r");
        if (!fp) {
                logger(LOG_ERROR, "Couldn't open \"%s\". Are you sure the file exists?\n", fname);
                return 0;
        }

        long flength = file_length(fp);

        char * fbuffer = malloc(flength + 1);
        fread(fbuffer, flength, 1, fp);

        // in windows files, there's a carriage return before newlines, but fread removes them for some reason
        // flength will therefore be too long as it counts bytes in the original file, not fbuffer
        if (OPERATING_SYSTEM == OS_WINDOWS) {
                flength -= chars_in_string(fbuffer, '\n', flength);
        }
        fbuffer[flength] = 0;

        fclose(fp);
        logger(LOG_SUCCESS, "Done moving \"%s\" to a string.\n", fname);
        return fbuffer;
}

struct Buffer file_to_buffer(char * fname)
{
        logger(LOG_INFO, "Moving the contents of \"%s\" to a buffer ...\n", fname);

        FILE * fp = fopen(fname, "r");
        if (!fp) {
                logger(LOG_ERROR, "Couldn't open \"%s\". Are you sure the file exists?\n", fname);
                return (struct Buffer) {0, -1};
        }

        struct Buffer buffer;
        buffer.size = file_length(fp);

        buffer.contents = malloc(buffer.size);
        fread(buffer.contents, buffer.size, 1, fp);

        logger(LOG_SUCCESS, "Done moving the contents of \"%s\" to a buffer.\n", fname);
        return buffer;
}

char write_buffer_to_file(struct Buffer * buffer, char * fpath)
{
        logger(LOG_INFO, "Moving the contents of a buffer to \"%s\".\n", fpath);

        FILE * fp = fopen(fpath, "w");
        if (!fp) {
                logger(LOG_ERROR, "Couldn't open \"%s\" for writing.\n", fpath);
                return 0;
        }

        fwrite(buffer->contents, 1, buffer->size, fp);
        fclose(fp);

        logger(LOG_SUCCESS, "Done moving the contents of a buffer to \"%s\".\n", fpath);
        return 1;
}
