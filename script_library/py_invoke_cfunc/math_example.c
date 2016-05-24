/*
 * compile to so library file for test
 * gcc -fPIC -shared -o libmath_example.so math_example.c
 */
#include "math_example.h"

int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}
