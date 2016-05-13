/*
 * Three events:IO/Timer/Signal
 * input Ctrl+C, program will exit.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include <ev.h>

#define CRLF	"\x0d\x0a"
#define TOTAL	5

void io_cb(struct ev_loop *main_loop, ev_io * io_w, int e)
{
	ssize_t n;
	char buf[1024];

	memset(buf, 0, sizeof(buf));

	puts("IO Callback");

	n = read(STDIN_FILENO, buf, sizeof(buf));
	if (n < 0) {
		printf("read failed: %s\n", strerror(errno));
	}
	buf[strlen(buf)] = '\0';

	printf("String: %s\n", buf);

	ev_io_stop(main_loop, io_w);
}

void timer_cb(struct ev_loop *main_loop, ev_timer *time_w, int e)
{
	puts("Timer Callback");
	ev_timer_stop(main_loop, time_w);
}

void signal_cb(struct ev_loop *main_loop, ev_signal *signal_w, int e)
{
	puts("Signal Callback");

	ev_signal_stop(main_loop, signal_w);

	ev_break(main_loop, EVBREAK_ALL);
}

void periodic_cb(struct ev_loop *main_loop, ev_periodic *periodic_w, int e)
{
	time_t now;
	static int count = TOTAL;

	now = time(NULL);
	printf("Periodic Callback, and now:%lu\n", now);

	count--;
	if (count == 0) {
		printf("trigger %d times, periodic watch will stop" CRLF
			   "Now you can input 'Ctrl+C' to stop program" CRLF, TOTAL);
		ev_periodic_stop(main_loop, periodic_w);
	}
}

void child_cb(struct ev_loop *main_loop, ev_child *child_w, int e)
{
	puts("Child Callback");

	ev_child_stop(main_loop, child_w);
}

void stat_cb(struct ev_loop *main_loop, ev_stat *stat_w, int e)
{
	puts("file change callback");

	ev_stat_stop(main_loop, stat_w);
}

int main(int argc, char **argv)
{
	int			fd;
	pid_t 		pid;
	int			child;
	ssize_t		n;
	const char	*msg = "Hello Libev!\n";
	const char	*file_name = "ev_stat_test";

	ev_io 		io_w;
	ev_timer 	timer_w;
	ev_signal	signal_w;
	ev_periodic	periodic_w;
	ev_stat		stat_w;
	ev_child	child_w;

	struct ev_loop *main_loop = ev_default_loop(0);

	ev_init(&io_w, io_cb);
	ev_io_set(&io_w, STDIN_FILENO, EV_READ);

	ev_init(&timer_w, timer_cb);
	ev_timer_set(&timer_w, 2, 0);

	ev_init(&signal_w, signal_cb);
	ev_signal_set(&signal_w, SIGINT);

	//ev_init(&periodic_w, periodic_cb);
	//ev_periodic_set(&periodic_w, 0., 3600., periodic_cb);
	// periodic is a cron-like watcher
	// next line means after 5 secs from now, periodic_cb will be triggered.
	// the third param is interval secs between two trigger events
   	//ev_periodic_init(&periodic_w, periodic_cb, time(NULL) + 5.0, 3., NULL);
   	ev_periodic_init(&periodic_w, periodic_cb, 0., 3., NULL);

	// watch file change event
	ev_stat_init(&stat_w, stat_cb, file_name, 0);
	
	// watch child exist event
	child = 0;
	pid = fork();
	switch (pid) {
	case -1:
		printf("fork failed: %s\n", strerror(errno));
		break;

	case 0:
		// child
		child = 1;
		break;

	default:
		break;
	}

	if (pid != -1 && child) {

		sleep(4);

		fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd < 0) {
			printf("open file '%s' failed: %s\n", file_name, strerror(errno));
			_exit(0);
		}

		printf("msg len: %u\n", strlen(msg));
		n = write(fd, msg, strlen(msg));
		if (n < 0) {
			printf("write file '%s' failed: %s\n", file_name, strerror(errno));
		}

		close(fd);

		sleep(6);

		_exit(0);
	}
	ev_child_init(&child_w, child_cb, pid, 0);

	ev_io_start(main_loop, &io_w);
	ev_timer_start(main_loop, &timer_w);
	ev_signal_start(main_loop, &signal_w);
	ev_periodic_start(main_loop, &periodic_w);
	ev_stat_start(main_loop, &stat_w);
	ev_child_start(main_loop, &child_w);

	ev_run(main_loop, 0);

	return 0;
}
