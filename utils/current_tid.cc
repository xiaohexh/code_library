//  g++ -o current_tid -g current_tid.cc -lpthread
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>

using std::cout;
using std::endl;

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

__thread int t_cachedId = 0;

namespace CurrentThread
{

void cacheTid()
{
	cout << "enter CurrentThread::cacheTid()" << endl;
	if (t_cachedId == 0) {
		t_cachedId = static_cast<pid_t>(::syscall(SYS_gettid));
	}
}

int tid()
{
	if (unlikely(t_cachedId == 0)) {
		cacheTid();
	}

	return t_cachedId;
}

};



void *threadFunc(void *arg)
{
	int *name = static_cast<int *>(arg);
	cout << "thread " << *name << " tid:" << CurrentThread::tid() << endl;
	sleep(1);
	cout << "thread " << *name << " tid:" << CurrentThread::tid() << endl;
	delete name;
}

int main(int argc, char **argv)
{
	for (int i = 0; i < 2; i++) {
		pthread_t pid;
		int *idx = new int(i);
		pthread_create(&pid, NULL, threadFunc, idx);
	}

	// ugly!! use countdownlatch under production evironment
	sleep(3);

	return 0;
}
