#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "xthread.h"

static int count = 0;

static xmutex_t mutex;
static xcond_t	cond;

X_THREAD_PROC(test_proc)
{
	char *msg = (char *)thr_arg;
	printf("args msg: %s\n", msg);

	X_LOCK(mutex);
	count++;
	X_COND_SIGNAL(cond);
	X_UNLOCK(mutex);
}

int main(int argc, char **argv)
{
	int status;
	xthread_t tid;

	X_MUTEX_CREATE(mutex);
	X_COND_CREATE(cond);

	status = xthread_create(&tid, test_proc, "Hello");
	if (status < 0) {
		printf("create thread failed\n");
	}

	X_LOCK(mutex);
	while (count == 0) {
		X_COND_WAIT(cond, mutex);
	}
	X_UNLOCK(mutex);

	printf("count current value: %d\n", count);

	return 0;
}
