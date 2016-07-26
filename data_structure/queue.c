#include "queue.h"

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

/**
 * queue_init - allocates a new FIFO using a preallocated buffer
 * @buffer: the preallocated buffer to be used.
 * @size: the size of the internal buffer, this have to be a power of 2.
 * @gfp_mask: get_free_pages mask, passed to kmalloc()
 * @lock: the lock to be used to protect the fifo buffer
 *
 * Do NOT pass the queue to queue_free() after use! Simply free the
  * &struct queue with free().
 */
struct queue *queue_init(unsigned char *buffer, unsigned int size)
{
    struct queue *fifo;

    /* size must be a power of 2 */
    //BUG_ON(!is_power_of_2(size));

    fifo = malloc(sizeof(struct queue));
    if (!fifo)
        return NULL;

    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = fifo->out = 0;

    return fifo;
}

/**
 * queue_alloc - allocates a new FIFO and its internal buffer
 * @size: the size of the internal buffer to be allocated.
 * @gfp_mask: get_free_pages mask, passed to kmalloc()
 * @lock: the lock to be used to protect the fifo buffer
 *
 * The size will be rounded-up to a power of 2.
 */
struct queue *queue_alloc(unsigned int size)
{
    unsigned char *buffer;
    struct queue *ret;

    /*
     * round up to the next power of 2, since our 'let the indices
     * wrap' technique works only in this case.
     */
    if (!is_power_of_2(size)) {
        size = roundup_pow_of_two(size);
    }

    buffer = malloc(size);
    if (!buffer)
        return NULL;

    ret = queue_init(buffer, size);

    if (ret == NULL) {
        free(buffer);
    }

    return ret;
}

/**
 * queue_free - frees the FIFO
  * @fifo: the fifo to be freed.
 */
void queue_free(struct queue *fifo)
{
    free(fifo->buffer);
    free(fifo);
}

/**
 * _queue_in - puts some data into the FIFO, no locking version
 * @fifo: the fifo to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int _queue_in(struct queue *fifo,
            const unsigned char *buffer, unsigned int len)
{
    unsigned int l;

    len = min(len, fifo->size - fifo->in + fifo->out);

    /* first put the data starting from fifo->in to buffer end */
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(fifo->buffer, buffer + l, len - l);

    fifo->in += len;

    return len;
}

/**
 * _queue_out - gets some data from the FIFO, no locking version
 * @fifo: the fifo to be used.
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most @len bytes from the FIFO into the
 * @buffer and returns the number of copied bytes.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int _queue_out(struct queue *fifo,
             unsigned char *buffer, unsigned int len)
{
    unsigned int l;
	
    len = min(len, fifo->in - fifo->out);

    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);

    fifo->out += len;

    return len;
}

#if 1
/* test queue */

struct student {
	int		age;
	char	*name;
};

struct student students[] = {
	{ 34, "Jean"  },
	{ 23, "Nil"   },
	{ 29, "Brian" },
	{ 0,  NULL    }
};

int main(int argc, char **argv)
{
	int i;
	int status;

	struct student s;

	size_t len, size;
	struct queue *q;

	len = sizeof(students) / sizeof(*students);
	size = len * sizeof(struct student);

	q = queue_alloc(size);

	for (i = 0; students[i].age != 0; i++) {
		queue_in(q, (unsigned char *)&students[i], sizeof(struct student));
	}

	while (1) {
		status = queue_out(q, (unsigned char *)&s, sizeof(struct student));
		if (status <= 0) {
			printf("queue is empty\n");
			break;
		}
		printf("name:%s, age:%d\n", s.name, s.age);
	}

	queue_free(q);

	size_t n = roundup_pow_of_two(120);
	printf("next pow of 2 %lu\n", n);

	if (is_power_of_2(8)) {
		printf("bingo\n");
	}

	return 0;

}

#endif
