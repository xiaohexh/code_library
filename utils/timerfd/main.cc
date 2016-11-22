/*
 * File:   main.cpp
 * Author: Administrator
 */

#include <cstdlib>
#include <iostream>

#include "timer_poll.h"

using namespace std;

int callback(timer &ptimer)
{
    printf("timer id=%d:%s\n", ptimer.timer_get_id(), (char *) ptimer.timer_get_userdata());
    return 0;
}

void *thread_fun(void *data)
{
    timers_poll *my_timers = (timers_poll *)data;
    my_timers->run();
}

/*
 *
 */
int main(int argc, char** argv)
{
    timers_poll my_timers(128);
    pthread_t thread_id = 0;
    pthread_create(&thread_id, NULL, thread_fun, &my_timers);

   
    timer timer1(1.05, callback, (void *) "hello 1",0);
    timer timer2(1.10, callback, (void *) "hello 2",0);

   // timer1.timer_start();
   // timer2.timer_start();

    my_timers.timers_poll_add_timer(timer1);
    my_timers.timers_poll_add_timer(timer2);

    sleep(5);
    my_timers.timers_poll_del_timer(timer2);
    cout<<"del complete"<<endl;
    timer1.timer_modify_internal(5.1);
    //timer2.timer_modify_internal(10.1);
    cout<<"modify complete"<<endl;
    sleep(4);
    //my_timers.timers_poll_del_timer(timer2);

    //sleep(5);

    //my_timers.timers_poll_deactive();

    pthread_join(thread_id,NULL);
    return 0;
}
