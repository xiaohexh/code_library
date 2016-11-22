#include "timestamp.h"

#include <iostream>
#include <sys/time.h>

TimeStamp::TimeStamp()
{
	struct timeval tv;
	::gettimeofday(&tv, NULL);
	_seconds = tv.tv_sec;
	_microSeconds = tv.tv_usec;
}

TimeStamp::TimeStamp(unsigned int secs, unsigned int microSecs) :
	_seconds(secs),
	_microSeconds(microSecs)
{
}

int main()
{
	TimeStamp ts;
	std::cout << ts.getSeconds() << std::endl;
	std::cout << ts.getMilliSeconds() << std::endl;
	std::cout << ts.getMicroSeconds() << std::endl;

	return 0;
}
