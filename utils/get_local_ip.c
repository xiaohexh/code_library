#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_SIZE 16

inline char* getIP(char *ip)
{
	if (ip == NULL) return NULL;

	int socket_fd;
	// struct sockaddr_in *sin;
	struct ifreq* ifr;
	struct ifconf conf;
	char buff[BUFSIZ];
	int  num;
	int  i;

	memset(ip, 0, sizeof(ip));

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	conf.ifc_len = BUFSIZ;
	conf.ifc_buf = buff;
	ioctl(socket_fd, SIOCGIFCONF, &conf);
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;

	for (i = 0; i < num; i++) {
		struct sockaddr_in* sin = (struct sockaddr_in*)(&ifr->ifr_addr);
		ioctl(socket_fd, SIOCGIFFLAGS, ifr);
		if (((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP)) {
			sprintf(ip, "%s", inet_ntoa(sin->sin_addr));
        	//ip.assign(inet_ntoa(sin->sin_addr));
            return ip;
		}
    	ifr++;
	}

	return NULL;
}

int main(int argc, char **argv)
{
	char *ret;
	char *ip = (char *)malloc(IP_SIZE);
	if (ip != NULL) {
		ret = getIP(ip);
		if (ret != NULL) {
			printf("ip:%s\n", ip);
			free(ip);
		}
	}

	return 0;
}
