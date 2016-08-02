#include "LayoutsFactory.h"

static LayoutsFactory *layouts_factory_ = 0;

std::auto_ptr<Layout> create_simple_layout(void *param);
std::auto_ptr<Layout> create_basic_layout(void *param);

LayoutsFactory &LayoutsFactory::getInstance()
{
	if (!layouts_factory_) {
		std::auto_ptr<LayoutsFactory> If(new LayoutsFactory);
		If->registerCreator("simple", &create_simple_layout);
		If->registerCreator("basic", &create_basic_layout);
		layouts_factory_ = If.release();
	}

	return *layouts_factory_;
}

void LayoutsFactory::registerCreator(const std::string &class_name,
		create_function_t create_function)
{
	const_iterator i = creators_.find(class_name);
	if (i != creators_.end()) {
		return;
	}

	creators_[class_name] = create_function;
}

std::auto_ptr<Layout> LayoutsFactory::create(const std::string &class_name,
		void *param)
{
	const_iterator i = creators_.find(class_name);
	if (i == creators_.end()) {
		/* TODO: throw exception */
	}

	return (*i->second)(param);
}

bool LayoutsFactory::registed(const std::string &class_name) const
{
	return creators_.find(class_name) == creators_.end();
}
