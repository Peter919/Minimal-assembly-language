// "out" here refers to the output
// since the screen needs to be cleared (out might change,
// after all), it's very unefficient to print out after
// every instruction so it uses a delay instead

#include <stdlib.h>
#include <time.h>
#include "log.h"
#include "settings.h"
#include "os.h"

#if OPERATING_SYSTEM == OS_WINDOWS
#include <windows.h>
#define SLEEP(seconds) Sleep(seconds * 1000)
#elif OPERATING_SYSTEM == OS_UNIX
#include <unistd.h>
#define SLEEP(seconds) sleep(seconds)
#else
#   error "Unknown system"
#endif

static void clear_screen(void)
{
#if OPERATING_SYSTEM == OS_WINDOWS
        system("cls");
#elif OPERATING_SYSTEM == OS_UNIX
        system("clear");
#else
#error "Unknown operating system"
#endif
}

#define b "BLINK"

void print_out(void * out)
{
        // little optimization so it doesn't print
        // out if there's nothing to print
        // also good for debugging so the screen
        // isn't cleared all the time
        if (! *(char *) out) {
                return;
        }

        static clock_t last_output = 0;
        if (clock() - last_output < g_seconds_between_printings * CLOCKS_PER_SEC) {
                return;
        }

        last_output = clock();
        clear_screen();
        logger(LOG_NONE, "%s", (char *) out);

        if (g_pause_seconds_after_print != 0) {
                SLEEP(g_pause_seconds_after_print);
        }
}
