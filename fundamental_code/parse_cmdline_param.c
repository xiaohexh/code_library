/*
 * compile: gcc -o parse_cmdline_param -g parse_cmdline_param.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

static int show_help;
static int show_version;
static int daemonize;
char *conf_file;
char *pid_file;
static int nprocs;

#define CRLF		"\x0d\x0a"
#define CONF_PATH	"./log"
#define PID_FILE	"./pid"
#define PROC_NUM	10

static struct option long_options[] = {
	{ "help",		no_argument,	NULL,	'h' },
	{ "version",	no_argument,	NULL,	'v' },
	{ "daemonize",	no_argument,	NULL,	'd' },
	{ "conf-file",	no_argument,	NULL,	'c' },
	{ "pid-file",	no_argument,	NULL,	'p' },
	{ "nprocs",		no_argument,	NULL,	'n' },
	{ NULL,			0,				NULL,	0   }
};

static const char *short_options = "hvdc:p:n:";

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
			show_version = 1;
			show_help = 1;
			break;

		case 'v':
			show_version = 1;
			break;

		case 'd':
			daemonize = 1;
			break;

		case 'c':
			conf_file = optarg;
			break;

		case 'p':
			pid_file = optarg;
			break;

		case 'n':
			nprocs = atoi(optarg);
			break;

		default:
			// errlog
			printf("invalid option '%c'\n", c);
			return -1;
		}
	}

	return 0;
}

void my_daemonize()
{
	printf("process will run as daemonize!\n");
}

static void show_usage(void)
{
    printf(
        "Usage: nutcracker [-hvd] [-c conf file] [-o output file]" CRLF
        "                  [-p pid file] [-m mbuf size]" CRLF
        "");
    printf(
        "Options:" CRLF
        "  -h, --help             : this help" CRLF
        "  -V, --version          : show version and exit" CRLF
        "  -d, --daemonize        : run as a daemon" CRLF);
    printf(
        "  -c, --conf-file=S      : set configuration file (default: %s)" CRLF
        "  -p, --pid-file=S       : set pid file (default: %s)" CRLF
        "  -n, --process-num=N    : set process number (default: %d)" CRLF
        "", 
        CONF_PATH,
        PID_FILE != NULL ? PID_FILE : "off",
        PROC_NUM);
}

void set_default_options()
{
	conf_file = CONF_PATH;
	pid_file = PID_FILE;
	nprocs = PROC_NUM;
}

int main(int argc, char **argv)
{
	int status;

	set_default_options();

	status = get_options(argc, argv);
	if (status < 0) {
		printf("parse common line params failed!\n");
		//return 1;
	}

	if (show_version) {
		printf("This is Version 1.0.0\n");
		if (show_help) {
			show_usage();
		}
	}

	if (daemonize) {
		my_daemonize();
	}

	printf("fork %d processes\n", nprocs);
	printf("pid file path %s\n", pid_file);

	return 0;
}
