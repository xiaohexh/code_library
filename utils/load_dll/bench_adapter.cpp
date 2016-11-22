#include "bench_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#define PROCTECT_AREA_SIZE (512*1024)

handle_func_t hfunc = {NULL};

int load_bench_adapter(const char *file, bool isGlobal)
{
	if (hfunc.handle != NULL) {
		dlclose(hfunc.handle);
	}
	
	memset(&hfunc, 0, sizeof(hfunc));

	int flag = isGlobal ? RTLD_NOW | RTLD_GLOBAL : RTLD_NOW;

	/* 提早发现部分内存写乱现象 */                                        
	mmap(NULL, PROCTECT_AREA_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);       
	void* handle = dlopen(file, flag);
	mmap(NULL, PROCTECT_AREA_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);   

	if (handle == NULL) {
		printf("dlopen '%s' failed: %s\n", file, strerror(errno));
		exit(-1);
	}

	void *func = dlsym(handle, "work_handle_init");
	if (func != NULL) {
		hfunc.work_handle_init = (work_handle_init_t)dlsym(handle, "work_handle_init");
		hfunc.work_handle_process = (work_handle_process_t)dlsym(handle, "work_handle_process");
		hfunc.work_handle_fini = (work_handle_fini_t)dlsym(handle, "work_handle_fini");

		if (hfunc.work_handle_process == NULL) {
			printf("work_handle_process not implement is not allowed\n");
			exit(-1);
		}

		hfunc.handle = handle;

		return 0;

	} else {
		if (hfunc.work_handle_init == NULL) {
			printf("work_handle_init not implement is not allowed\n");
			return -1;
		}
	}
}
