/*
 * A simple queue implementation.
 * Not multi-thread safe.
 */
#ifndef _QUEUE_H
#define _QUEUE_H

#define min(a, b) ((a < b) ? (a) : (b))

struct queue {
	unsigned char *buffer;	/* the buffer holding the data */
	unsigned int size;	/* the size of the allocated buffer */
	unsigned int in;	/* data is added at offset (in % size) */
	unsigned int out;	/* data is extracted from off. (out % size) */
};

extern struct queue *queue_init(unsigned char *buffer, unsigned int size);
extern struct queue *queue_alloc(unsigned int size);
extern void queue_free(struct queue *fifo);
extern unsigned int _queue_in(struct queue *fifo,
				const unsigned char *buffer, unsigned int len);
extern unsigned int _queue_out(struct queue *fifo,
				unsigned char *buffer, unsigned int len);

/**
 * __queue_reset - removes the entire FIFO contents, no locking version
 * @fifo: the fifo to be emptied.
 */
static inline void _queue_reset(struct queue *fifo)
{
	fifo->in = fifo->out = 0;
}

/**
 * queue_reset - removes the entire FIFO contents
 * @fifo: the fifo to be emptied.
 */
static inline void queue_reset(struct queue *fifo)
{
	unsigned long flags;

	_queue_reset(fifo);
}

/**
 * queue_in - puts some data into the FIFO
 * @fifo: the fifo to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 */
static inline unsigned int queue_in(struct queue *fifo,
				const unsigned char *buffer, unsigned int len)
{
	unsigned long flags;
	unsigned int ret;

	ret = _queue_in(fifo, buffer, len);

	return ret;
}

/**
 * queue_out - gets some data from the FIFO
 * @fifo: the fifo to be used.
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most @len bytes from the FIFO into the
 * @buffer and returns the number of copied bytes.
 */
static inline unsigned int queue_out(struct queue *fifo,
				     unsigned char *buffer, unsigned int len)
{
	unsigned long flags;
	unsigned int ret;

	ret = _queue_out(fifo, buffer, len);

	/*
	 * optimization: if the FIFO is empty, set the indices to 0
	 * so we don't wrap the next time
	 */
	if (fifo->in == fifo->out)
		fifo->in = fifo->out = 0;

	return ret;
}

/**
 * __queue_len - returns the number of bytes available in the FIFO, no locking version
 * @fifo: the fifo to be used.
 */
static inline unsigned int _queue_len(struct queue *fifo)
{
	return fifo->in - fifo->out;
}

/**
 * queue_len - returns the number of bytes available in the FIFO
 * @fifo: the fifo to be used.
 */
static inline unsigned int queue_len(struct queue *fifo)
{
	unsigned long flags;
	unsigned int ret;

	ret = _queue_len(fifo);

	return ret;
}

#endif
