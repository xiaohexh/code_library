#ifndef _SIMPLE_LAYOUT_H_
#define _SIMPLE_LAYOUT_H_

#include "Layout.h"
#include <memory>

class SimpleLayout : public Layout
{
public:
	SimpleLayout();

	virtual std::string format(const LoggingEvent &event);
};

#endif
