#include "my_signal.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "my_log.h"

static struct signal signals[] = { 
    { SIGINT,   "SIGINT",   0,  sig_handler },
    { SIGTERM,  "SIGTERM",  0,  sig_handler },
    { SIGUSR1,  "SIGUSR1",  0,  sig_handler },
    { 0,        NULL,       0,  NULL }
};

int sig_init()
{
    int status;
    struct signal *sig;

    for (sig = signals; sig->signo != 0; sig++) {
        struct sigaction sa; 

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig->handler;
        sa.sa_flags = sig->flags;
        sigemptyset(&sa.sa_mask);

        status = sigaction(sig->signo, &sa, NULL);
        if (status < 0) {
            log_error("sigaction failed: %s", strerror(errno));
            return MY_ERROR;
        }
    }

    return MY_OK;
}

void sig_handler(int signo, sig_cb_t cb, void *data)
{
    switch (signo) {
    case SIGINT:
    case SIGTERM:
        log_stderr("I've received SIGINT sig, and I will exist");
        cb(data);
        break;

    case SIGUSR1:
        log_stderr("I've received SIGUSR1, just for testing.");
        break;

    default:
        log_error("I've received %d, but I don't know what to do?", signo);
        break;
    }
}
