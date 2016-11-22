#ifndef _API_PLUS_H_
#define _API_PLUS_H_

typedef int (*work_handle_init_t)(void *, void *);
typedef int (*work_handle_process_t)(unsigned, void *, void *);
typedef int (*work_handle_fini_t)(void *, void *);

struct handle_func_t {
	void *handle;
	work_handle_init_t work_handle_init;
	work_handle_process_t work_handle_process;
	work_handle_fini_t work_handle_fini;
};

extern handle_func_t hfunc;

#endif
