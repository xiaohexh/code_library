#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>

#include "bench_adapter.h"

#define CRLF	"\x0d\x0a"

#define MODULE_PATH	"work_handle.so"
#define CONFIG_PATH	"work_handle_cfg.ini"

static int show_usage = 0;
static char *conf_file = NULL;
static char *module_file = NULL;

static struct option long_options[] = {
	{ "help",		no_argument,	NULL,	'h' },
	{ "conf-file",	no_argument,	NULL,	'c' },
	{ "module-file",no_argument,	NULL,	'm' },
	{ NULL,			0,				NULL,	0   }
};

static const char *short_options = "hc:m:";

int get_options(int argc, char **argv)
{
	int c;

	for (;;) {
		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h':
			show_usage = 1;
			break;

		case 'c':
			conf_file = optarg;
			break;

		case 'm':
			module_file = optarg;
			break;

		default:
			// errlog
			printf("invalid option '%c'\n", c);
			return -1;
		}
	}

	return 0;
}

void set_default_options()
{
	conf_file = CONFIG_PATH;
	module_file = MODULE_PATH;
}

void show_help(char *bin_name)
{
	printf("usage:" CRLF
		   "%s [-c config_file] [-m module_file]\n", basename(bin_name));

	printf("default [-c work_handle_cfg.ini] [-m work_handle.so]\n");
}

int main(int argc, char **argv)
{
	int status;

	int init_ret;
	int proc_ret;
	int fini_ret;

	set_default_options();

	status = get_options(argc, argv);
	if (status < 0) {
		printf("parse cmd line parameters failed\n");
		return 1;
	}

	if (show_usage) {
		show_help(argv[0]);
		return 0;
	}

	status = load_bench_adapter(module_file, true);
	if (status != 0) {
		printf("load_bench_adapter failed!\n");
		return 1;
	}

	/*
	 * init and process cannot be null,
	 * this is guaranteed by load_bench_adapter
	 */
	init_ret = hfunc.work_handle_init((void *)conf_file, NULL);
	if (init_ret < 0) {
		printf("work_handle_init failed\n");
		return 1;
	}

	proc_ret = hfunc.work_handle_process(0, NULL, NULL);
	if (proc_ret < 0) {
		printf("work_handle_process failed\n");
		return 1;
	}

	if (hfunc.work_handle_process != NULL) {
		fini_ret = hfunc.work_handle_fini(NULL, NULL);
		if (fini_ret < 0) {
			printf("work_handle_fini failed\n");
			return 1;
		}
	}

	return 0;
}
