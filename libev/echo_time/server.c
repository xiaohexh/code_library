#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include <ev.h>

#include "my_log.h"
#include "my_signal.h"

#define LOG_F	"./et.log"

struct ev_loop	*main_loop;
ev_io read_w;
ev_io write_w;

static void server_in(struct ev_loop *loop, ev_io *io_w, int events);
void server_stop();

int set_nonblocking(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFD);
	if (flags < 0) {
		log_error("fcntl F_GETFD failed: %s", strerror(errno));
		return MY_ERROR;
	}

	return fcntl(fd, F_SETFD, flags | O_NONBLOCK);
}

static void server_out(struct ev_loop *loop, ev_io *io_w, int events)
{
	int		status;
	ssize_t n;
	char	buf[1024];
	struct timeval tv;

	memset(buf, 0, sizeof(buf));

	status = gettimeofday(&tv, NULL);
	if (status < 0) {
		snprintf(buf, sizeof(buf), "get current time failed: %s", strerror(errno)); 
		log_error("%s", buf);
	} else {
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
	}

	n = write(io_w->fd, buf, strlen(buf));
	if (n < 0) {
		log_error("write to '%d' failed: %s", io_w->fd, strerror(errno));
	}

	ev_io_stop(main_loop, io_w);

	ev_io_init(&read_w, server_in, io_w->fd, EV_READ);
	ev_io_start(main_loop, &read_w);
}

static void server_in(struct ev_loop *loop, ev_io *io_w, int events)
{
	ssize_t n;
	char	buf[1024];

	log_debug(LOG_DEBUG, "enter server_in and read from client");

	ssize_t	size = sizeof(buf);
	ssize_t recvd = 0;

	memset(buf, 0, sizeof(buf));

	for (;;) {
		n = read(io_w->fd, buf + recvd, size - recvd);
		if (n < 0) {
			if (errno == EINTR) {
				continue;
			}
			break;
		}

		if (n > 0) {
			ev_io_stop(main_loop, io_w);
			log_debug(LOG_INFO, "read msg from client: %s", buf);
			ev_io_init(&write_w, server_out, io_w->fd, EV_WRITE);
			ev_io_start(main_loop, &write_w);
			break;
		}

		if (n == 0) {
			log_debug(LOG_INFO, "read msg from client: %s", buf);
			ev_io_stop(main_loop, io_w);
			close(io_w->fd);
			return;
		}

		log_error("read from '%d' failed: %s", strerror(errno));
		return;
	}
}

static void server_accept(struct ev_loop *loop, ev_io *io_w, int events)
{
	int sd;
	
	sd = accept(io_w->fd, NULL, NULL);
	if (sd < 0) {
		log_error("accept failed: %s", strerror(errno));
		return;
	}

	log_debug(LOG_DEBUG, "accept client connection success");
	set_nonblocking(sd);

	ev_io_init(&read_w, server_in, sd, EV_READ);
	ev_io_start(main_loop, &read_w);
}

void server_stop()
{
	ev_break(main_loop, EVBREAK_ALL);
}

int main(int argc, char **argv)
{
	int			status;

	int			sd;
	uint16_t	port = 8080;
	const char	*host = "127.0.0.1";
	struct sockaddr_in servaddr;

	/* init log */
	status = log_init(LOG_DEBUG, LOG_F);

	/* init signal handler */
	sig_init();

	/* create listening socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		log_error("socket failed: %s", strerror(errno));
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, host, &servaddr.sin_addr);

	status = set_nonblocking(sd);
	if (status != MY_OK) {
		close(sd);
		log_deinit();
		return 1;
	}

	status = bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (status < 0) {
		log_error("bind '%d' failed: %s", strerror(errno));
		close(sd);
		log_deinit();
		return 1;
	}

	status = listen(sd, 1024);
	if (status < 0) {
		log_error("listen '%d' failed: %s", strerror(errno));
		close(sd);
		log_deinit();
		return 1;
	}

	/* register ev event and begin to listen */
	ev_io accept_w;
	main_loop = ev_default_loop(0);
	ev_io_init(&accept_w, server_accept, sd, EV_READ);
	ev_io_start(main_loop, &accept_w);

	ev_run(main_loop, 0);

	close(sd);
	log_deinit();

	return 0;
}
