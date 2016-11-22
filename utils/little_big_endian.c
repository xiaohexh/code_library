#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * gcc -o little_big_endian -g little_big_endian.c
 /

unsigned char ecb_byteorder_helper (void)
{
  /* the union code still generates code under pressure in gcc, */
  /* but less than using pointers, and always seems to */
  /* successfully return a constant. */
  /* the reason why we have this horrible preprocessor mess */
  /* is to avoid it in all cases, at least on common architectures */
  /* or when using a recent enough gcc version (>= 4.6) */
#if __i386 || __i386__ || _M_X86 || __amd64 || __amd64__ || _M_X64
  return 0x44;
#elif __BYTE_ORDER__ && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return 0x44;
#elif __BYTE_ORDER__ && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return 0x11;
#else
  union
  {
    uint32_t i;
    uint8_t c;
  } u = { 0x11223344 };
  return u.c;
#endif
}

int ecb_big_endian    (void) { return ecb_byteorder_helper () == 0x11; }
int ecb_little_endian (void) { return ecb_byteorder_helper () == 0x44; }

int main(int argc, char **argv)
{
	if (ecb_big_endian()) {
		printf("big endian\n");
	} else {
		printf("little endian\n");
	}

	return 0;
}
