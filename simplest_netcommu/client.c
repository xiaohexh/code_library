#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_BUF_LEN	1024

int main(int argc, char **argv)
{
	int status;
	int sd;
	int port;
	char *host;

	char rbuf[MAX_BUF_LEN];
	char wbuf[MAX_BUF_LEN];

	ssize_t n;

	struct sockaddr_in servaddr;

	host = argv[1];
	port = atoi(argv[2]);

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		printf("socket failed: %s\n", strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	status = inet_pton(AF_INET, host, &servaddr.sin_addr);
	if (status < 0) {
		printf("inet_pton failed: %s\n", strerror(errno));
		goto err;
	}
	servaddr.sin_port = htons(port);

	status = connect(sd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (status < 0) {
		printf("connect failed: %s\n", strerror(errno));
		goto err;
	}

	memset(rbuf, 0, sizeof(rbuf));
	memset(wbuf, 0, sizeof(wbuf));

	snprintf(wbuf, sizeof(wbuf), "%s", "hello server");
	n = write(sd, wbuf, sizeof(wbuf));
	if (n < 0) {
		printf("write failed: %s\n", strerror(errno));
		goto err;
	}

	n = read(sd, rbuf, sizeof(rbuf));
	if (n < 0) {
		printf("read failed: %s\n", strerror(errno));
		goto err;
	}

	printf("read msg from server: %s\n", rbuf);

err:
	close(sd);
	return -1;

	close(sd);
	return 0;
}
