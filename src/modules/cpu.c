
#include "../dwmstatus.h"
#include <stdio.h>
#include <string.h>

typedef struct {
	/* 0:cpu 1:user 2:unice 3:system 4:idle 5:iowait 6:irq 7:softirq 8:steal 9:guest 10:? */
	unsigned long long user;
	unsigned long long nice;
	unsigned long long system;
	unsigned long long idle;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
	unsigned long long steal;
	unsigned long long guest;

	unsigned long long total;
} CpuStat;

CpuStat stat, prev_stat;


// Functions
// =========
void read_cpustats();


// Public
// ======

void cpu_init()
{
    read_cpustats();
}

int
cpu_update(char * const str, int n, const usecs_t usecs)
{
	unsigned long long maxval;
	int percent;

    prev_stat = stat;
	read_cpustats();

	maxval = stat.total - prev_stat.total;

	//::BUGFIX divide by 0 error::
	if (maxval != 0)
	    percent = 100 * (maxval - (stat.idle - prev_stat.idle)) / maxval;
    else
        percent = 0;

    return snprintf(str, n, "%2i%%", percent);
}


// Private
// =======

void
read_cpustats()
{
	FILE *fp;
	char line[1024];

	fp = xfopenr("/proc/stat");

	memset(&stat, 0, sizeof(CpuStat));

	while (fgets(line, sizeof(line), fp))
	{
		if (line[0] == 'c' && line[1] == 'p' && line[2] == 'u' && line[3] == ' ')
		{
			sscanf(line, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu",
					&stat.user, &stat.nice, &stat.system, &stat.idle, &stat.iowait,
			        &stat.irq, &stat.softirq, &stat.steal, &stat.guest
			);
			stat.total = stat.user + stat.nice + stat.system + stat.idle + stat.iowait +
                         stat.irq + stat.softirq + stat.steal + stat.guest;
		}
	}
	fclose(fp);
}

