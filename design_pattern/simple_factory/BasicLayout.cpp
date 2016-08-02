#include "BasicLayout.h"

BasicLayout::BasicLayout()
{
}

std::string BasicLayout::format(const LoggingEvent &event)
{
	return std::string("BasicLayout");
}

std::auto_ptr<Layout> create_basic_layout(void *param)
{
	return std::auto_ptr<Layout>(new BasicLayout());
}
