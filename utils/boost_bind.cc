#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using std::cout;
using std::endl;

class Animal
{
public:
	int foo(double price);
};

int Animal::foo(double price)
{
	cout << "Animal::foo price:" << price << endl;
}

class Bar
{
public:
	int foo(double price);
};

int Bar::foo(double price)
{
	cout << "Bar::foo price:" << price << endl;
}

typedef boost::function<int (double)> Foo;

int main(int argc, char **argv)
{
	Animal a;
	Bar b;

	Foo foo;
	foo = boost::bind(&Animal::foo, &a, _1);
	foo(45.3);

	foo = boost::bind(&Bar::foo, &b, _1);
	foo(67.89);

	return 0;
}
