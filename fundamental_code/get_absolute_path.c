#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
	char *ppath;
	char path[1024];

	memset(path, 0, sizeof(path));

	ppath = getcwd(path, sizeof(path));
	if (ppath != NULL) {
		printf("current work dir: %s\n", ppath);
		printf("current work dir: %s\n", path);
	} else {
		printf("getcwd failed: %s\n", strerror(errno));
	}

	return 0;
}
