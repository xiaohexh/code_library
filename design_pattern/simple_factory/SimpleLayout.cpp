#include "SimpleLayout.h"

SimpleLayout::SimpleLayout()
{
}

std::string SimpleLayout::format(const LoggingEvent &event)
{
	return std::string("SimpleLayout");
}

std::auto_ptr<Layout> create_simple_layout(void *param)
{
	return std::auto_ptr<Layout>(new SimpleLayout());
}
