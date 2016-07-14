#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>

#define MIN_RESERVED_FDS	32

int main(int argc, char **argv)
{
	int maxclients = 50000;
	rlim_t maxfiles = maxclients + MIN_RESERVED_FDS;
	struct rlimit limit;

	if (getrlimit(RLIMIT_NOFILE, &limit) < 0) {
		printf("getrlimit failed: %s\n", strerror(errno));
		maxclients = 1024 - MIN_RESERVED_FDS;
	} else {
		rlim_t oldlimit = limit.rlim_cur;

		if (oldlimit < maxfiles) {
			rlim_t f;
			int setrlimit_error = 0;

			f = maxfiles;
			while (f > oldlimit) {
				int decr_step = 16;

				limit.rlim_cur = f;
				limit.rlim_max = f;
				if (setrlimit(RLIMIT_NOFILE, &limit) != -1) break;
				setrlimit_error = errno;

				if (f < decr_step) break;
				f -= decr_step;
			}

			if (f < oldlimit) f = oldlimit;

			if (f != maxfiles) {
				int old_maxclients = maxclients;
				maxclients = f - MIN_RESERVED_FDS;
				if (maxclients < 1) {
					printf("your current ulimit -n is not enough, will exist\n");
					exit(1);
				}
			}

			printf("set open files to %llu(originally %llu)\n",
					(unsigned long long)maxfiles,
					(unsigned long long)oldlimit);
		}
	}

	return 0;
}
