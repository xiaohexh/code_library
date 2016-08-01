/*
 * compile cmd: g++ -o pthreads_demo -g pthreads_demo.cpp PThreads.cpp -lpthread
 */
#include <iostream>
#include <string>
#include <string.h>

#include "PThreads.hh"

class Demo
{
public:
	Demo();

	void setThreadId();
	char *getThreadId();

private:
	mutable threading::Mutex _mutex;
	char _pthreadId[16];
};

Demo::Demo()
{
	memset(_pthreadId, 0, sizeof(_pthreadId));
}

void Demo::setThreadId()
{
	threading::ScopedLock lock(_mutex);
	snprintf(_pthreadId, sizeof(_pthreadId), "%lu", pthread_self());
}

char *Demo::getThreadId()
{
	threading::ScopedLock lock(_mutex);
	return _pthreadId;
}

void *process(void *arg)
{
	Demo d;
	d.setThreadId();
	printf("%s\n", d.getThreadId());
}

int main()
{
	pthread_t pid;
	pthread_create(&pid, NULL, process, NULL);

	std::cout << "pid:" << pid << std::endl;

	pthread_join(pid, NULL);

	return 0;

}
