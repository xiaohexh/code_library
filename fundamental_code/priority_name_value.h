#ifndef _PRIORITY_NAME_VALUE_H_
#define _PRIORITY_NAME_VALUE_H_

#include <string>
#include <stdexcept>

class Priority
{
public:
	static const int MESSAGE_SIZE;  // = 8

	typedef enum {
		EMERG = 0,
		FATAL = 0,
		ALERT = 100,
		CRIT = 200,
		ERROR = 300,
		WARN = 400,
		NOTICE = 500,
		INFO = 600,
		DEBUG = 700,
		NOTSET = 800
	} PriorityLevel;

	/* The type of Priority Values */
	typedef int Value;

	static const std::string &getPriorityName(int priority) throw();

	static Value getPriorityValue(const std::string &priorityName)
	throw(std::invalid_argument);
};

#endif
