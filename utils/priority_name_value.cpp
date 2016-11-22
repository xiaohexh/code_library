#include "priority_name_value.h"

#include <iostream>
#include <exception>
#include <cstdlib>

namespace {
	const std::string *names()
	{
		static const std::string priority_names[10] = {
			"FATAL",
			"ALERT",
			"CRIT",
			"ERROR",
			"WARN",
			"NOTICE",
			"INFO",
			"DEBUG",
			"NOTSET",
			"UNKNOWN" 
		};
		return priority_names;
	}
}

const int Priority::MESSAGE_SIZE = 8;

const std::string &Priority::getPriorityName(int priority) throw()
{
	priority++;
	priority /= 100;
	return names()[(((priority < 0) || (priority > 8)) ? 8 : priority)];
}

Priority::Value Priority::getPriorityValue(const std::string &priorityName)
throw(std::invalid_argument)
{
	Priority::Value value = -1;

	for (unsigned int i = 0; i < 10; i++) {
		if (priorityName == names()[i]) {
			value = i * 100;
			break;
		}
	}

	if (value == -1) {
		if (priorityName == "EMERG") {
			value = 0;
		} else {
			char *endPointer;
			value = strtoul(priorityName.c_str(), &endPointer, 10);
			if (*endPointer != 0) {
				throw std::invalid_argument(
					std::string("unknown priority name: '") + priorityName + "'"
				);
			}
		}
	}

	return value;
}

int main()
{
	std::cout << Priority::getPriorityName(Priority::DEBUG) << std::endl;
	std::cout << Priority::getPriorityValue("ALERT") << std::endl;

	std::cout << Priority::getPriorityName(900) << std::endl;
	std::cout << Priority::getPriorityValue("800") << std::endl;
	std::cout << Priority::getPriorityValue("DBUG") << std::endl;

	return 0;
}
