/*
 * gcc -o roundup_pow_of_2 -g roundup_pow_of_2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define is_power_of_2(n)	\
     (n != 0 && ((n & (n - 1)) == 0))

static inline int fls(int x)  
{  
    int position;  
    int i;  
    if(0 != x) {
        for (i = (x >> 1), position = 0; i != 0; ++position)  
            i >>= 1;  
    } else {   
        position = -1;  
    }    
    return position+1;  
}  

/** 
 * fls64 - find last bit set in a 64-bit value 
 * @n: the value to search 
 * 
 * This is defined the same way as ffs: 
 * - return 64..1 to indicate bit 63..0 most significant bit set 
 * - return 0 to indicate no bits set 
 */  
static inline int fls64(uint64_t x)  
{  
    uint32_t h = x >> 32;  
    if (h)  
        return fls(h) + 32;  
    return fls(x);  
}  
  
static inline unsigned long roundup_pow_of_two(unsigned long x)  
{  
	unsigned a;

    if (sizeof(x - 1) == 4) {
         a = fls(x - 1);
	} else {
    	a = fls64(x - 1);  
	}

    return 1UL << a;
}

int main(int argc, char **argv)
{
	unsigned long x = 23;
	unsigned long next = roundup_pow_of_two(x);
	printf(" 23 next power of 2 is %lu\n", next);

	return 0;
}
