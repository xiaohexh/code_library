#ifndef _LAYOUTS_FACTORY_H_
#define _LAYOUTS_FACTORY_H_

#include <map>
#include <memory>
#include "Layout.h"

class LayoutsFactory
{
public:
	typedef std::auto_ptr<Layout> (*create_function_t)(void *param);

	static LayoutsFactory &getInstance();
	void registerCreator(const std::string &class_name, create_function_t create_function);
	std::auto_ptr<Layout> create(const std::string &class_name, void *param);
	bool registed(const std::string &class_name) const;

private:
	LayoutsFactory() {};
	LayoutsFactory(LayoutsFactory &);
	LayoutsFactory &operator = (LayoutsFactory &);

	typedef std::map<std::string, create_function_t> creators_t;
	typedef creators_t::const_iterator const_iterator;

	creators_t creators_;
};

#endif
