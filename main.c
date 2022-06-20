#include <stdio.h>
#include <string.h>

#if OPERATING_SYSTEM == OS_WINDOWS
#include <conio.h>
#endif

#include "log.h"
#include "os.h"
#include "file_handling.h"
#include "compiler.h"
#include "run.h"

// argv = path to this file, path to program file, "c" for compile, "r" for run, "cr" for compile run
int main(int argc, char * argv[])
{
        if (argc != 3) {
                logger(LOG_FATAL_ERROR, "Wrong argc: %d, expected 3.\n", argc);
                goto PROGRAM_END;
        }

        char * dirpath = get_dirpath(argv[1]);

        if (!dirpath) {
                logger(LOG_FATAL_ERROR, "Expected argv[1] to be a filepath, got \"%s\".\n", argv[1]);
                goto PROGRAM_END;
        }

        if (strchr(argv[2], 'c')) {
                if (!compile(dirpath)) {
                        logger(LOG_FATAL_ERROR, "Encountered an error while compiling \"%s\".\n", dirpath);
                        goto PROGRAM_END;
                }
        }

        if (strchr(argv[2], 'r')) {
                run(dirpath);
        }

PROGRAM_END:
        if (OPERATING_SYSTEM == OS_WINDOWS) {
                logger(LOG_NONE, "Press any key to continue.\n");
                getch();
        } else {
                logger(LOG_NONE, "Press enter to continue.\n");
                getchar();
        }
        return 0;
}
