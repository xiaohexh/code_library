#include "../singleton.h"

#include "add_class.h"
#include <stdio.h>

#define ADD_CONFIG	"add_module.ini"

extern "C" int work_handle_init()
{
	int status;
	status = Singleton<AddClass>::instance()->init(ADD_CONFIG);
	if (status < 0) {
		printf("AddClass init failed\n");
		return -1;
	}

	return 0;
}

extern "C" int work_handle_process()
{
	int ret;
	int a, b;

	a = 4;
	b = 6;
	ret = Singleton<AddClass>::instance()->add(a, b);
	printf("AddClass add %d and %d is %d\n", a, b, ret);

	return 0;
}

extern "C" int work_handle_fini()
{
	printf("Do nothing just test fini!\n");

	return 0;
}
