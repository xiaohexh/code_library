#ifndef _MY_SIGNAL_H_
#define _MY_SIGNAL_H_

#include <signal.h>

typedef void (*sig_cb_t)(void *data);

struct signal {
    int     signo;
    char    *signame;
    int     flags;
    void (*handler)(int signo);
};

int sig_init();
void sig_handler(int signo);

#endif
