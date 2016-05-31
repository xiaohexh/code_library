#include "add_class.h"

#include <stdio.h>

AddClass::AddClass()
{
}

AddClass::~AddClass()
{
}

int AddClass::init(const char *file)
{
	printf("AddClass init config file: %s\n", file);
	return 0;
}

int AddClass::add(int a, int b)
{
	return a + b;
}
