#ifndef _BASIC_LAYOUT_H_
#define _BASIC_LAYOUT_H_

#include "Layout.h"
#include <memory>

class BasicLayout : public Layout
{
public:
	BasicLayout();

	virtual std::string format(const LoggingEvent &event);
};

#endif
