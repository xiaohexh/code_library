#include "my_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <errno.h>

static struct logger logger;

int log_init(int level, char *name)
{
	struct logger *l = &logger;

	int fd;
	fd = open(name, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd < 0) {
		_log_stderr("open log file failed: %s", strerror(errno));
		return MY_ERROR;
	}

	l->fd = fd;
	l->name = name;
	l->level = level;

	return MY_OK;
}

int log_deinit()
{
	struct logger *l = &logger;

	int status;

	if (l->fd > 0) {
		status = close(l->fd);
		if (status < 0) {
			_log_stderr("close log file failed: %s", strerror(errno));
			return MY_ERROR;
		}
	}

	return MY_OK;
}

void log_level_up(int levelname)
{
}

void log_level_down(int level)
{
}

int log_loggable(int level)
{
	struct logger *l = &logger;

	return (level > l->level) ? 0 : 1;
}

void _log(const char *name, int line, const char *fmt, ...)
{
	struct logger *l = &logger;

	char buf[LOG_MAX_LEN];
	ssize_t n;
	int len, size;
	struct timeval tv;
	va_list args;

	len = 0;
	size = LOG_MAX_LEN;

	gettimeofday(&tv, NULL);
	len += strftime(buf + len, size - len, "[%Y-%m-%d %H:%M:%S]", localtime(&tv.tv_sec));
	len += snprintf(buf + len, size - len, "[%s %d] ", name, line);

	va_start(args, fmt);
	len += vsnprintf(buf + len, size - len, fmt, args);
	va_end(args);

	buf[len++] = '\n';

	n = write(l->fd, buf, len);
	if (n < 0) {
		_log_stderr("write log to log file failed: %s", strerror(errno));
	}
}


void _log_stderr(const char *fmt, ...)
{
	char buf[LOG_MAX_LEN];
	int len, size;
	struct timeval tv;
	ssize_t n;
	va_list args;

	len = 0;
	size = LOG_MAX_LEN;

	gettimeofday(&tv, NULL);
	len += strftime(buf + len, size - len, "[%Y-%m-%d %H:%M:%S] ", localtime(&tv.tv_sec));

	va_start(args, fmt);
	len += vsnprintf(buf + len, size - len, fmt, args);
	va_end(args);

	buf[len++] = '\n';

	n = write(STDERR_FILENO, buf, len);
	if (n < 0) {
		printf("write failed: %s\n", strerror(errno));
	}
}
