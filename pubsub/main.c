#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <linux/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ae.h"

#define PORT	10001 
#define BACKLOG	5

void read_query_from_client(aeEventLoop *el, int fd, void *privdata, int mask);

struct server {
	aeEventLoop *el;
	int listenfd;
};

struct server server;

int set_tcpnodelay(int fd)
{
	int nodelay;
	socklen_t len;

	nodelay = 1;
	len = sizeof(nodelay);

	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, len);
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
	int flag;

	flag = fcntl(fd, F_GETFL);
	if (flag < 0) {
		return -1;
	}

	return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

int server_cron(struct aeEventLoop *evnetLoop, long long id, void *clientData)
{
	time_t now = time(NULL);
	//printf("Hello, server_cron now: %lu\n", now);
	return 5000;
}

void send_resp_to_client(aeEventLoop *el, int fd, void *privdata, int mask)
{
	int n, status;
	char *resp = "Hello, I am server!";

	n = write(fd, resp, strlen(resp));
	if (n < 0) {
		if (errno == EAGAIN) {
			n = 0;
		} else {
			close(fd);
		}
        printf("write to client failed!\n");
	}

	printf("send to client success\n");

	aeDeleteFileEvent(el, fd, AE_WRITABLE);

	status = aeCreateFileEvent(el, fd, AE_READABLE, read_query_from_client, NULL);
	if (status < 0) {
		printf("send_resp_to_client aeCreateFileEvent failed!\n");
	}
}

void read_query_from_client(aeEventLoop *el, int fd, void *privdata, int mask)
{
	printf("read from client is triggered\n");

	char buf[128];

	int status;
	int n;

	n = read(fd, buf, sizeof(buf));
	if (n == 0) {
		printf("client close connection\n");
		close(fd);
		return;
	} else if (n < 0) {
		if (errno == EAGAIN) {
			n = 0;
		} else {
			printf("read from client failed: %s\n", strerror(errno));
			close(fd);
			return;
		}
	}
	if (n > 0) {
		printf("read from client: %s, read n:%d, size:%lu\n", buf, n, strlen(buf));
		aeDeleteFileEvent(el, fd, AE_READABLE);
    	status = aeCreateFileEvent(el, fd, AE_WRITABLE, send_resp_to_client, NULL);
    	if (status < 0) {
   			printf("read_query_from_client aeCreateFileEvent failed!\n");
    	}
	}
}

void accept_conn_from_client(aeEventLoop *el, int fd, void *privdata, int mask)
{
	int status;
	int connfd;

	struct sockaddr_in clientaddr;

	socklen_t len = sizeof(clientaddr);

	connfd = accept(fd, (struct sockaddr *)&clientaddr, &len);
	if (connfd < 0) {
		printf("accept failed: %s\n", strerror(errno));
		return;
	}

	status = set_nonblocking(connfd);
	if (status < 0) {
		printf("set nonblocking failed: %s\n", strerror(errno));
		return;
	}

	status = set_tcpnodelay(connfd);
	if (status < 0) {
		printf("set nodelay failed: %s\n", strerror(errno));
		return;
	}

	status = aeCreateFileEvent(el, connfd, AE_READABLE, read_query_from_client, NULL);
	if (status < 0) {
		printf("accept_conn_from_client aeCreateFileEvent failed!\n");
	}
}

void sigterm_handler(int signo)
{
	printf("recv stop signal, I will exit!\n");
	aeStop(server.el);
}

void setup_signal_handler(void)
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigterm_handler;
	sigaction(SIGINT, &sa, NULL);
}

int listen_to_port(struct server *s)
{
	int status;

	struct sockaddr_in servaddr;

	s->listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s->listenfd < 0) {
		printf("socket failed: %s\n", strerror(errno));
		return -1;
	}

	status = set_reuseaddr(s->listenfd);
	if (status < 0) {
		printf("reuseaddr failed: %s\n", strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT); 
	
	status = bind(s->listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (status < 0) {
		printf("bind failed: %s\n", strerror(errno));
		return -1;
	}

	status = listen(s->listenfd, BACKLOG);
	if (status < 0) {
		printf("listen failed: %s\n", strerror(errno));
		return -1;
	}

	status = set_nonblocking(s->listenfd);
	if (status < 0) {
		printf("set nonblocking failed: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int init_server(struct server *s)
{
	int status;

	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	setup_signal_handler();

	s->el = aeCreateEventLoop(1024);

	listen_to_port(s);

	status = aeCreateFileEvent(s->el, s->listenfd, AE_READABLE, accept_conn_from_client, NULL);

	status = aeCreateTimeEvent(s->el, 5000, server_cron, NULL, NULL);

	return status;
}

int main(int argc, char **argv)
{
	struct server *s = &server;
	init_server(s);

	aeMain(s->el);

	aeDeleteEventLoop(s->el);

	return 0;
}
