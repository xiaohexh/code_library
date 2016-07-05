#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_BUF_LEN	1024

typedef void (*sig_cb_t)(int signo);

static int stop = 0;

int register_signal(int signo, int flags, sig_cb_t cb) 
{
    int status;

    struct sigaction sa; 

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = cb; 
    sa.sa_flags = flags;
    sigemptyset(&sa.sa_mask);

    status = sigaction(signo, &sa, NULL);
    if (status < 0) {
        printf("sigaction failed: %s", strerror(errno));
        return -1; 
    }   

    return 0;
}

void set_stop(int signo)
{
	switch (signo) {
	case SIGTERM:
	case SIGINT:
		printf("recv SIG_TERM/SIG_INT, I will quit!\n");
		stop = 1;
	}
}

int setnonblocking(int fd)
{
	int flag;

	flag = fcntl(fd, F_GETFL);
	if (flag < 0) {
		printf("fcntl F_GETFL failed: %s\n", strerror(errno));
		return flag;
	}

	return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

int main(int argc, char **argv)
{
	int status;
	int listen_sd;
	int port;
	char *host;

	char rbuf[MAX_BUF_LEN];
	char wbuf[MAX_BUF_LEN];

	ssize_t n;

	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;

	if (argc < 3) {
		printf("usage: %s host port\n", basename(argv[0]));
		return 0;
	}

	host = argv[1];
	port = atoi(argv[2]);

	status = register_signal(SIGTERM, 0, set_stop);
	if (status < 0) {
		return -1;
	}

	status = register_signal(SIGINT, 0, set_stop);
	if (status < 0) {
		return -1;
	}

	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0) {
		printf("socket failed: %s\n", strerror(errno));
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	status = inet_pton(AF_INET, host, &servaddr.sin_addr);
	if (status < 0) {
		printf("inet_pton failed: %s\n", strerror(errno));
		return -1;
	}
	servaddr.sin_port = htons(port);

	status = bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (status < 0) {
		printf("bind failed: %s\n", strerror(errno));
		return -1;
	}

	status = listen(listen_sd, 1024);
	if (status < 0) {
		printf("listen failed: %s\n", strerror(errno));
		return -1;
	}

	/*
	status = setnonblocking(listen_sd);
	if (status < 0) {
		return -1;
	}
	*/

	memset(rbuf, 0, sizeof(rbuf));
	memset(wbuf, 0, sizeof(wbuf));

	while (!stop) {

		char client[MAX_BUF_LEN];
		int conn_sd;
		socklen_t len = sizeof(clientaddr);

		conn_sd = accept(listen_sd, (struct sockaddr *)&clientaddr, &len);
		//conn_sd = accept(listen_sd, NULL, NULL);
		if (conn_sd < 0) {
			printf("accept failed: %s\n", strerror(errno));
			continue;
		}

		inet_ntop(AF_INET, &clientaddr.sin_addr, client, sizeof(client));

		printf("recv connection from client: %s\n", client);

		n = read(conn_sd, rbuf, sizeof(rbuf));
		if (n < 0) {
			printf("read failed: %s\n", strerror(errno));
			close(conn_sd);
			continue;
		}

		printf("read msg from client: %s\n", rbuf);

		snprintf(wbuf, sizeof(wbuf), "%s", "hello client, I am server!");
		n = write(conn_sd, wbuf, sizeof(wbuf));
		if (n < 0) {
			printf("write failed: %s\n", strerror(errno));
			close(conn_sd);
			continue;
		}
	}

	close(listen_sd);

	return 0;
}
