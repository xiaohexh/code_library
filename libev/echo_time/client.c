/*
 * gcc -o client -g client.c my_log.c 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#include "my_log.h"

#define LOG_F	"./et.log"

int main(int argc, char **argv)
{
	int status;
	ssize_t n;

	int	sd;
	int port;
	const char *host = "127.0.0.1";
	struct sockaddr_in servaddr;
	socklen_t	len;
	const char *msg = "Hello\n";
	char	buf[1024];

	//port = 8080;
	port = 22122;

	log_init(LOG_DEBUG, LOG_F);

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		log_error("socket failed: %s", strerror(errno));
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, host, &servaddr.sin_addr);

	len = sizeof(servaddr);

	log_debug(LOG_DEBUG, "client before connect");
	status = connect(sd, (struct sockaddr *)&servaddr, len);
	if (status < 0) {
		log_error("connect failed: %s", strerror(errno));
		close(sd);
		log_deinit();
		return 1;
	}

	sleep(4);

	log_debug(LOG_DEBUG, "client connect success");
	n = send(sd, msg, strlen(msg), 0);
	if (n < 0) {
		log_error("send '%s' to server failed: %s", msg, strerror(errno));
		close(sd);
		log_deinit();
		return 1;
	}

	log_debug(LOG_DEBUG, "send '%s' to server success", msg);

	memset(buf, 0, sizeof(buf));

	log_debug(LOG_DEBUG, "client before read from server");
	n = read(sd, buf, sizeof(buf));
	log_debug(LOG_DEBUG, "client after read from server");

	log_stderr("resp from server: %s", buf);

	sleep(5);

	close(sd);

	log_deinit();

	return 0;
}
