#include "LoggingEvent.h"

LoggingEvent::LoggingEvent(const std::string &category,
	const std::string &msg,
	int priority) :
	categoryName(category),
	message(msg),
	priority(priority)
{
}
