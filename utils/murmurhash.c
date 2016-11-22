#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

uint32_t _MurmurHash(const void *key, int len, uint32_t seed);

int IdHash(const void *key, int len)
{
    int seed = 31;  // never change
    int hash = (int)_MurmurHash(key, len, seed);
    return abs(hash);
} 

uint32_t _MurmurHash(const void *key, int len, uint32_t seed)
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24; 

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16; 
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };  
  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

int main(int argc, char **argv)
{
	const char *msg = "_13982394fzq_sh+chris";
	int ret;
	
	ret = IdHash(msg, strlen(msg));
	printf("hash ret:%d\n", ret);

	return 0;
}
