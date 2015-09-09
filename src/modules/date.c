
#include "../dwmstatus.h"
#include "../../config.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


// Public
// ======

void date_init()
{
}

int
date_update(char * const str, int n, const usecs_t usecs)
{
	static char buf[256];
	time_t rtime;

	time(&rtime);

	strftime(buf, sizeof(buf), DATE_FORMAT, localtime(&rtime));

    return snprintf(str, n, "%s", buf);
}

