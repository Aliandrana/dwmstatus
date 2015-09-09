
#include "../dwmstatus.h"
#include <stdio.h>
#include <string.h>

typedef struct {
	unsigned int total;
	unsigned int used;	
	unsigned int free;
	unsigned int buffers;
	unsigned int cached;

	/* -/+ buffers/cache */
	unsigned int real_used;
	unsigned int real_free;

	unsigned int swap_total;
	unsigned int swap_used;
	unsigned int swap_free;
} MemStat;

MemStat memstat;


// Functions
// =========
void read_memstat();


// Public
// ======

void
memory_init()
{
}

int
memory_update(char * const str, int n, const usecs_t usecs)
{
	read_memstat();

	return snprintf(str, n, "[%4u / %4u MiB]", memstat.real_used/1024, memstat.used/1024);
}


// Private
// =======

void
read_memstat()
{
	FILE *fp;
	char buf[50];

	fp = xfopenr("/proc/meminfo");

	while (fscanf(fp, "%50s:", buf) == 1)
	{
		if (strcmp(buf, "MemTotal:") == 0)
			fscanf(fp, "%u kB", &memstat.total);
		if (strcmp(buf, "MemFree:") == 0)
			fscanf(fp, "%u kB", &memstat.free);
		if (strcmp(buf, "Buffers:") == 0)
			fscanf(fp, "%u kB", &memstat.buffers);
		if (strcmp(buf, "Cached:") == 0)
			fscanf(fp, "%u kB", &memstat.cached);

		if (strcmp(buf, "SwapTotal:") == 0)
			fscanf(fp, "%u kB", &memstat.swap_total);
		if (strcmp(buf, "SwapFree:") == 0)
			fscanf(fp, "%u kB", &memstat.swap_used);
	}
	
	memstat.used = memstat.total - memstat.free;
	memstat.real_used = memstat.used - memstat.buffers - memstat.cached;
	memstat.real_free = memstat.used + memstat.buffers + memstat.cached;
	memstat.swap_free = memstat.swap_total - memstat.swap_used;

	fclose(fp);
}

