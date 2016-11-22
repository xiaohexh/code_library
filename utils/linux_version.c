# include <stdio.h>
# include <stdlib.h>
# include <sys/utsname.h>

static unsigned int linux_version (void)
{
#ifdef __linux
  unsigned int v = 0;
  struct utsname buf;
  int i;
  char *p = buf.release;

  if (uname (&buf))
    return 0;

  //printf("%s\n", p);

  for (i = 3+1; --i; ) {

      unsigned int c = 0;

      for (;;) {
          if (*p >= '0' && *p <= '9') {
            c = c * 10 + *p++ - '0';
		  } else {
              p += *p == '.';
              break;
          }
      }

      v = (v << 8) | c;
	  //printf("c = %u, v = %u\n", c, v);
    }

  return v;
#else
  return 0;
#endif
}

int main(int argc, char **argv)
{
	printf("linux ver: %u\n", linux_version());

	return 0;
}
