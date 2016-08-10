#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define OK 0
#define ERR -1

#define PORT	11011
#define BACKLOG 1024
#define NEVENTS 1024

#define MAX_BUF_SIZE 16384

/* Anti-warning macro... */
#define NOTUSED(V) ((void) V)

struct event_base {
    int		ep;		/* epoll descriptor */
    struct epoll_event *event;	/* event[] -- event triggered */
    int nevent;		/* # event */
};

struct server {
    int ld;		/* listen fd */
    struct event_base *evb;
};

static int stop = 0;

struct server server;

void signal_handler(int signo)
{
    NOTUSED(signo);
    stop = 1;
}

void register_signal(int signo)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;
    sigaction(signo, &sa, NULL);
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

    flag = fcntl(fd, F_GETFD);
    if (flag < 0) {
        printf("fcntl with F_GETFD failed: %s\n", strerror(errno));
        return ERR;
    }

    return fcntl(fd, F_SETFD, flag | O_NONBLOCK);
}

/*
int set_recvbuf(int fd)
{
	int recv_buf;
	socklen_t len;

	recv_buf = 32 * 1024;
	len = sizeof(recv_buf);

	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF,(const char*)&recv_buf, len);
}
*/

int listen_to_port()
{
    int status;
    struct sockaddr_in servaddr;

    server.ld = socket(AF_INET, SOCK_STREAM, 0);
    if (server.ld < 0) {
        printf("socket failed: %s\n", strerror(errno));
        return ERR;
    }

    status = set_reuseaddr(server.ld);
    if (status < 0) {
        printf("set reuseaddr failed: %s\n", strerror(errno));
        goto err;
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    status = bind(server.ld, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (status < 0) {
        printf("bind failed: %s\n", strerror(errno));
        goto err;
    }

    status = listen(server.ld, BACKLOG);
    if (status < 0) {
        printf("listen failed: %s\n", strerror(errno));
        goto err;
    }

    status = set_nonblocking(server.ld);
    if (status < 0) {
        printf("set nonblocking failed: %s\n", strerror(errno));
        goto err;
    }

    /* add listen socket fd to epoll */
    struct epoll_event event;
    event.events = (uint32_t)(EPOLLIN | EPOLLET);
    event.data.fd = server.ld;
    status = epoll_ctl(server.evb->ep, EPOLL_CTL_ADD, server.ld, &event);
    if (status < 0) {
        printf("epoll_ctl EPOLL_CTL_ADD sd %d failed: %s\n", server.ld, strerror(errno));
        goto err;
    }

    return OK;

err:
    close(server.ld);
    return ERR;
}

int accept_conn()
{
    printf("recv connection request from client\n");
    int client_sd;
    int status;

    client_sd = accept(server.ld, NULL, NULL);
    if (client_sd < 0) {
        printf("accept failed: %s\n", strerror(errno));
        return ERR;
    }

    status = set_nonblocking(client_sd);
    if (status < 0) {
        printf("set nonblocking failed: %s\n", strerror(errno));
        close(client_sd);
        return ERR;
    }

	/*
	status = set_recvbuf(client_sd);
    if (status < 0) {
        printf("set recvbuf failed: %s\n", strerror(errno));
        close(client_sd);
        return ERR;
    }
	*/

    /* add client socket fd to epoll */
    struct epoll_event event;
    //event.events = (uint32_t)(EPOLLIN | EPOLLOUT | EPOLLET);
    event.events = (uint32_t)(EPOLLIN | EPOLLET);
    event.data.fd = client_sd;
    status = epoll_ctl(server.evb->ep, EPOLL_CTL_ADD, client_sd, &event);
    if (status < 0) {
        printf("epoll_ctl EPOLL_CTL_ADD sd %d failed: %s\n", client_sd, strerror(errno));
        close(client_sd);
        return ERR;
    }

    return OK;
}

int recv_req(int sd)
{
    int status;
    ssize_t n = 0, recvd = 0;
    char buf[MAX_BUF_SIZE];
    size_t size = sizeof(buf);

    memset(buf, 0, size);

    for (;;) {
        n = read(sd, buf + recvd, size);
        if (n < 0) {

            if (errno = EAGAIN) {
                break;
            }

            if (errno == EINTR) {
                continue;
            }

            status = epoll_ctl(server.evb->ep, EPOLL_CTL_DEL, sd, NULL);
            if (status < 0) {
                printf("epoll_ctl DEL sd:%d from epoll failed\n", sd);
            }

            close(sd);

            return ERR;

        } else if (n == 0) {

            status = epoll_ctl(server.evb->ep, EPOLL_CTL_DEL, sd, NULL);
            if (status < 0) {
                printf("epoll_ctl DEL sd:%d from epoll failed\n", sd);
            }

            close(sd);

            break;

        } else {
            if (n < size) {  /* recv finish */
                break;
            }
            recvd += n;
        }
    }

    printf("recv msg from client: %s", buf);

    struct epoll_event event;
    event.events = (uint32_t)(EPOLLIN | EPOLLOUT | EPOLLET);
    event.data.fd = sd;
    status = epoll_ctl(server.evb->ep, EPOLL_CTL_MOD, sd, &event);

    return OK;
}

int send_resp(int sd)
{
    printf("send response to client\n");
    ssize_t n = 0, total = 0, sent = 0;
    int status;
    struct timeval tv;
    char buf[MAX_BUF_SIZE];
    size_t size = sizeof(buf);

    memset(buf, 0, size);

    gettimeofday(&tv, NULL);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S\n", localtime(&tv.tv_sec));

    total = strlen(buf);

    for (;;) {
        n = write(sd, buf + sent, total);
        if (n < 0) {
			if (errno == EINTR) {
				continue;
			}

			if (errno == EAGAIN) {
				usleep(100);		/* send buffer is full, sleep 100us, try again */
				continue;
			}

			return ERR;
		}

		if (n == total)
			break;

		total -= n;
		sent += n;
    }

    struct epoll_event event;
    event.events = (uint32_t)(EPOLLIN | EPOLLET);
    event.data.fd = sd;
    status = epoll_ctl(server.evb->ep, EPOLL_CTL_MOD, sd, &event);

    return OK;
}

int create_event_base()
{
    int event_fd;

    server.evb = (struct event_base *)malloc(sizeof(struct event_base));
    if (server.evb == NULL) {
        return ERR;
    }

    server.evb->nevent = NEVENTS;
    server.evb->event = (struct epoll_event *)calloc(sizeof(struct epoll_event), server.evb->nevent);
    if (server.evb->event == NULL) {
        free(server.evb);
        return ERR;
    }

    event_fd = epoll_create(server.evb->nevent);
    if (event_fd < 0) {
        free(server.evb->event);
        free(server.evb);
        return ERR;
    }

    server.evb->ep = event_fd;

    return OK;
}

int event_loop()
{
    int i;
    int nsd;
    while (!stop) {
        nsd = epoll_wait(server.evb->ep, server.evb->event, server.evb->nevent, -1);
        for (i = 0; i < nsd; i++) {
            struct epoll_event *ev = &server.evb->event[i];

            if (ev->events & EPOLLIN) {
                if (ev->data.fd == server.ld) {  /* listen fd triggerd */
                    accept_conn();
                } else {
                    recv_req(ev->data.fd);
                }
            }

            if (ev->events & EPOLLOUT) {
                send_resp(ev->data.fd);
            }

            continue;
        }

        if (nsd <= 0) {
            if (errno == EINTR) {
                continue;
            } else {
                printf("epoll wait on e %d failed: %s\n", server.evb->ep, strerror(errno));
                return -1;
            }
        }
    }
}

void delete_event_base()
{
    close(server.evb->ep);
    free(server.evb->event);
    free(server.evb);
}

int main(int argc, char **argv)
{
    int status;

    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    register_signal(SIGINT);
    register_signal(SIGTERM);

    status = create_event_base();
    if (status < 0) {
        goto error;
    }

    status = listen_to_port();
    if (status < 0) {
        goto error;
    }

    event_loop();

    delete_event_base();

    close(server.ld);

    return 0;

error:
    delete_event_base();
    return 1;
}
