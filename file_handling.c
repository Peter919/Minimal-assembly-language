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

        // on some operating systems, newlines takes 2 bytes of space
        // and it's hard to figure out what value flength really should
        // hold. but fread returns the amount of bytes read, so we can
        // get the correct flength no matter what
        char * fbuffer = malloc(flength + 1);
        long true_flength = fread(fbuffer, 1, flength, fp);
        fbuffer[true_flength] = 0;

        fclose(fp);

        // as explained, flength might be too long so fbuffer is larger than
        // what's needed. these two lines make sure no extra space is used
        char * fstring = malloc(strlen(fbuffer) + 1);
        strcpy(fstring, fbuffer);

        logger(LOG_SUCCESS, "Done moving \"%s\" to a string.\n", fname);
        return fstring;
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
        // as explained in file_to_string, file_length may be wrong
        buffer.contents = malloc(file_length(fp));
        buffer.size = fread(buffer.contents, 1, file_length(fp), fp);

        // don't malloc more than what's needed!
        char * old_contents = buffer.contents;
        buffer.contents = malloc(buffer.size);
        memcpy(buffer.contents, old_contents, buffer.size);
        free(old_contents);

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
