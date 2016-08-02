#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include <string>
#include "LoggingEvent.h"

class Layout
{
public:
	virtual ~Layout() {}

	virtual std::string format(const LoggingEvent &event) = 0;
};

#endif
