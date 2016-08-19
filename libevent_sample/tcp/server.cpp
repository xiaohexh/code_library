/**
 * display implement a tcp server with libevent library
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include <event.h>

#define MAX_BUFF_SIZE	1024

struct event_base *eb;

void on_read(int fd, short event, void *arg)
{
	int len;
	char buf[MAX_BUFF_SIZE];

	memset(buf, 0, sizeof(buf));

	len = recv(fd, buf, sizeof(buf), 0);
	if (len < 0) {

		if (errno == EINTR || errno == EAGAIN)
			return;

		printf("recv failed: %s\n", strerror(errno));
		close(fd);

	} else if (len == 0) {

		printf("client closed\n");

		// remove this connection from event base
		struct event *ev_read = (struct event *)arg;
		event_del(ev_read);
		delete ev_read;

		close(fd);

	} else {
		printf("recv msg from client: %s\n", buf);
	}
}

void on_accept(int sd, short event, void *arg)
{
	int clisd;
	char cliip[16];
	struct sockaddr_in cliaddr;

	socklen_t len = sizeof(cliaddr);

	clisd = accept(sd, (struct sockaddr *)&cliaddr, &len);
	if (clisd < 0) {
		printf("accept client conn failed: %s\n", strerror(errno));
		return;
	} else {
		memset(cliip, 0, sizeof(cliip));
		inet_ntop(AF_INET, (void *)&cliaddr, cliip, sizeof(cliip));
		printf("recved conn from client: %s\n", cliip);

		// add to event base
		struct event *ev = (struct event *)malloc(sizeof(struct event));
		if (ev == NULL) {
			printf("malloc struct event failed: %s\n", strerror(errno));
			return;
		}
		event_set(ev, clisd, EV_READ | EV_PERSIST, on_read, ev);
		event_base_set(eb, ev);
		event_add(ev, NULL);
	}
}

int set_reuseaddr(int fd)
{
	int reuse;
	socklen_t len;

	reuse = 1;
	len = sizeof(reuse);
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, len);
}

int set_nonblocking(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
		return flags;
	}

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
	int status;
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		return 1;
	}

	status = set_reuseaddr(sd);
	if (status < 0) {
		goto err;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8888);

	status = bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (status < 0) {
		goto err;
	}

	status = listen(sd, 1024);
	if (status < 0) {
		goto err;
	}

	status = set_nonblocking(sd);
	if (status < 0) {
		goto err;
	}

	eb = event_base_new();

	struct event ev;
	event_set(&ev, sd, EV_READ | EV_PERSIST, on_accept, NULL);
	event_base_set(eb, &ev);
	event_add(&ev, NULL);

	event_base_dispatch(eb);

	close(sd);

	return 0;

err:
	close(sd);
	return 1;
}

