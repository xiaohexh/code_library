#ifndef _LOGGING_EVENT_H_
#define _LOGGING_EVENT_H_

#include <string>

class LoggingEvent
{
public:
	LoggingEvent(const std::string &category,
			const std::string &msg,
			int priority);

private:
	const std::string categoryName;
	const std::string message;
	int priority;
};

#endif
