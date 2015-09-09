
#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE

#include "dwmstatus.h"
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>

typedef struct {
	struct timespec prev;
	struct timespec current;
} Timer;

typedef struct {
    void (*init)();
    int (*update)(char * const str, int n, const usecs_t usecs);    
} Module;

#include "../modules.gen.h"

#define LENGTH(X)   (sizeof X / sizeof X[0])


// Functions
// =========
void timer_setup(Timer *timer);
usecs_t timer_usecs(Timer *timer);
FILE * xfopenr(char *filename);


int
main(int argc, char *argv[])
{
    int i;
    Timer timer;
    Display *dpy;
    usecs_t usecs;
    char str[1024];
    char *ptr;
    int n, l;

    if (!(dpy = XOpenDisplay(NULL))) {
        fprintf(stderr, "Cannot open display.\n");
        exit(EXIT_FAILURE);
    }

    timer_setup(&timer);

    for (i = 0; i < LENGTH(MODULES); i++)
    {
        MODULES[i].init();
    }

    while (1)
    {
        n = LENGTH(str) - 2;
        str[0] = 0;
        ptr = str;

        usecs = timer_usecs(&timer);

        for (i = 0; i < LENGTH(MODULES); i++)
        {
            if (i > 0 && n > 0)
            {
                *ptr = ' ';
                ptr++;
                n--;
            }

            l = MODULES[i].update(ptr, n, usecs);

            if (l < 0)
            {
                *ptr = 0;
                break;
            }
            else if (l < n)
            {
                ptr += l;
                n -= l;
            }
            else {
                break;
            }
        }

        XStoreName(dpy, DefaultRootWindow(dpy), str);
        XSync(dpy, False);

        usleep(USLEEP_DELAY);
    }
}

void
timer_setup(Timer *timer)
{
	clock_gettime(CLOCK_MONOTONIC, &timer->prev);
}


usecs_t
timer_usecs(Timer *timer)
{
	usecs_t usecs;
	clock_gettime(CLOCK_MONOTONIC, &timer->current);

	usecs = (timer->current.tv_sec - timer->prev.tv_sec) * 1000000
	      + (timer->current.tv_nsec - timer->prev.tv_nsec + 500)/1000;

	timer->prev = timer->current;

	return usecs;
}

FILE *
xfopenr(char *filename)
{
	FILE *fp;

	if (!(fp = fopen(filename, "r")))
	{
		fprintf(stderr, "Unable to read %s", filename);		
		exit(EXIT_FAILURE);
	}
	else
	{
		return fp;
	}
}


