#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

static void simple_daemonize(void)
{
	int fd;

	if (fork() != 0) exit(0); /* parent exits */
	setsid();	/* create a new session */

	if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close(fd);
	}
}

static int my_daemonize(int dump_core)
{
	int status;
	pid_t pid, sid;
	int fd;

	pid = fork();
	switch (pid) {
	case -1:
		printf("fork failed: %s\n", strerror(errno));
		return -1;

	case 0:	// child
		break;

	default: // parent terminate
		_exit(0);
	}

	/* 1st child continues and becomes the session leader */
	sid = setsid();
	if (sid < 0) {
		printf("setsid() failed: %s\n", strerror(errno));
		return -1;
	}

    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
    	printf("signal(SIGHUP, SIG_IGN) failed: %s", strerror(errno));
        return -1;
    }

	pid = fork();
	switch (pid) {
	case -1:
		printf("fork failed: %s\n", strerror(errno));
		return -1;

	case 0:	// child
		break;

	default: // 1st child terminate
		_exit(0);
	}

	/* 2nd child continue */

    /* change working directory */
    if (dump_core == 0) {
        status = chdir("/");
        if (status < 0) {
            printf("chdir(\"/\") failed: %s", strerror(errno));
            return -1;
        }
    }

    /* clear file mode creation mask */
    umask(0);

    /* redirect stdin, stdout and stderr to "/dev/null" */
    fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        printf("open(\"/dev/null\") failed: %s", strerror(errno));
        return -1;
    }

    status = dup2(fd, STDIN_FILENO);
    if (status < 0) {
        printf("dup2(%d, STDIN) failed: %s", fd, strerror(errno));
        close(fd);
        return -1;
    }

    status = dup2(fd, STDOUT_FILENO);
    if (status < 0) {
        printf("dup2(%d, STDOUT) failed: %s", fd, strerror(errno));
        close(fd);
        return -1;
    }

    status = dup2(fd, STDERR_FILENO);
    if (status < 0) {
        printf("dup2(%d, STDERR) failed: %s", fd, strerror(errno));
        close(fd);
        return -1;
    }

    if (fd > STDERR_FILENO) {
    	status = close(fd);
    	if (status < 0) {
        	printf("close(%d) failed: %s", fd, strerror(errno));
        	return -1;
    	}
	}
	return 0;
}

int main(int argc, char **argv)
{
	printf("I will run on background\n");
	//my_daemonize(1);
	simple_daemonize();

	printf("U cannot see this line 'cauz code run under bg\n");
	sleep(10);

	return 0;
}
