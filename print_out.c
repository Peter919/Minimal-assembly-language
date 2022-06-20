// "out" here refers to the output
// since the screen needs to be cleared (out might change,
// after all), it's very unefficient to print out after
// every instruction so it uses a delay instead

#include <stdlib.h>
#include <time.h>
#include "log.h"
#include "os.h"

static void clear_screen(void)
{
#if OPERATING_SYSTEM == OS_WINDOWS
        system("cls");
#else
        system("clear");
#endif
}

void print_out(void * out, clock_t delay)
{
        // little optimization so it doesn't print
        // out if there's nothing to print
        // also good for debugging so the screen
        // isn't cleared all the time
        if (! *(char *) out) {
                return;
        }

        static clock_t last_output = 0;
        if (clock() - last_output < delay) {
                return;
        }

        last_output = clock();
        clear_screen();
        logger(LOG_NONE, "%s", (char *) out);
}
