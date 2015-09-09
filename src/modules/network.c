
#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE

#include "../dwmstatus.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


typedef struct {
	unsigned long long sent;
	unsigned long long recieved;
} NetStat;

NetStat netstat;
NetStat prev_netstat;

int prev_base;
usecs_t prev_base_usecs;

const char units[] = {' ', 'K', 'M', 'G' };
const int unit_values[] = { 1, 1024, 1048576, 1073741824 };

// useconds to keep the base of the network field
const int NET_KEEP_BASE = 1000000;


// Functions
// =========
void read_netstat();


// Public
// ======
void
network_init()
{
    prev_base = 0;
    prev_base_usecs = 0;

    read_netstat();
}

int
network_update(char * const str, int n, const usecs_t usecs)
{
	double up, down, bigger;
	int base;

    prev_netstat = netstat;
	read_netstat();

	if (usecs > 0)
	{
		up = 8 * (netstat.sent - prev_netstat.sent) * 1000000 / usecs;
		down = 8 * (netstat.recieved - prev_netstat.recieved) * 1000000 / usecs;

		bigger = up > down ? up : down;
		base = 0;

		while (bigger > 900 && base < (sizeof(units) - 1))
		{
			base++;
			bigger /= 1024;
		}

        if (base >= prev_base)
        {
            prev_base = base;
            prev_base_usecs = NET_KEEP_BASE;
        }
        else if (prev_base_usecs >= 0)
        {
            base = prev_base;
            prev_base_usecs -= usecs;
        }

        up /= unit_values[base];
        down /= unit_values[base];

		return snprintf(str, n, "%7.2lf /%7.2lf %cbps", up, down, units[base]);
	}

    return 0;
}

// Private
// =======

void
read_netstat()
{
	FILE *fp;
	char line[1024];
	char dev[12];
	unsigned long long recieved, sent;

	fp = xfopenr("/proc/net/dev");	

	netstat.recieved = 0;
	netstat.sent = 0;
	while(fgets(line, sizeof(line), fp))
	{
		if (sscanf(line, "%12s%llu%*u%*u%*u%*u%*u%*u%*u%llu", dev, &recieved, &sent) == 3)
		{
			if (strcmp(dev, "lo:") != 0)
			{
				netstat.recieved += recieved;
				netstat.sent += sent;
			}
		}
	}

	fclose(fp);
}

