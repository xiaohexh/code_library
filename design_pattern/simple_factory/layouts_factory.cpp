#include "LayoutsFactory.h"

#include <iostream>

int main(int argc, char **argv)
{
	LoggingEvent event("a", "b", 1);

	std::auto_ptr<Layout> sl = LayoutsFactory::getInstance().create("simple", NULL);
	std::cout << sl->format(event) << std::endl;

	std::auto_ptr<Layout> bl = LayoutsFactory::getInstance().create("basic", NULL);
	std::cout << bl->format(event) << std::endl;

	return 0;
}
