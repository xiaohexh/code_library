#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
 * gcc -o useful_macro -g useful_macro.c
 */


/*
 * distinguish C++ and C
 */
#ifdef __cplusplus
#define XX_CPP(x) x
#else
#define XX_CPP(x)
#endif

/*
 * forbid throw exception
 */
#define XX_THROW XX_CPP(throw())

XX_CPP(extern "C" {)

int add(int a, int b) XX_THROW;

XX_CPP(})

#if 0
/* test: distinguish C++ and C */
int main(int argc, char **argv)
{
	int a, b;

	a = 3;
	b = 4;
	printf("%d + %d = %d\n", a, b, add(a, b));

	return 0;
}

int add(int a, int b) XX_THROW
{
	return a + b;
}
#endif

/*
 * transform to string
 */
#define STRINGIFY(s) # s
#define CONCAT(a, b) a ## b
#if 0
/* test: STRINGIFY(s) # s */
int main(int argc, char **argv)
{
	printf("%s\n", STRINGIFY(3.1415));
	printf("%d\n", CONCAT(3, 4));

	return 0;
}
#endif


/*
 * macro define base and derived class
 */
/* reference lighttpd */
#if 0
#define DATA_UNSET 			
	data_type_t type; 		\
    buffer *key; 			\
    int is_index_key; /* 1 if key is a array index */ \
    struct data_unset *(*copy)(const struct data_unset *src); \
    void (* free)(struct data_unset *p);	\
    void (* reset)(struct data_unset *p);	\
    int (*insert_dup)(struct data_unset *dst, struct data_unset *src); \
   	void (*print)(const struct data_unset *p, int depth)

struct data_integer {
	DATA_UNSET;

	int value;
};

struct data_config {
	DATA_UNSET;

	int context_idx;
	char *value;
};

#endif

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif
#if 0
int main(int argc, char **argv)
{
	int a, b;

	a = 1;
	b = 2;
	if (likely( a == 1)) {
		printf("You guess right, a == 1\n");
	}

	if (unlikely(b != 1)) {
		printf("You guess right, b != 1\n");
	}

	return 0;
}

#endif

#ifndef __offsetof
#define __offsetof(type, field) ((size_t)(&((type *)NULL)->field))
#endif

#if 0
struct student {
	int 	age;
	char	*name;
	double	score;
};

int main(int argc, char **argv)
{
	struct student s = { 23, "Jean", 89.9 };

	printf("score offset in struct student:%u\n", __offsetof(struct student, score));
	//printf("name:%d\n", (int *)(&s));

	return 0;
}
#endif

/*
 * test n is power of 2 or not
 */
#define is_power_of_2(n)    \
      (n != 0 && ((n & (n - 1)) == 0))

#if 0
int main(int argc, char **argv)
{
	unsigned long n = 254;
	if (is_power_of_2(n)) {
		printf("%lu is power of 2\n", n);
	} else {
		printf("%lu is not power of 2\n", n);
	}
	return 0;
}
#endif

/*
 * include different file according to macro defined.
 */
/* Include the best multiplexing layer supported by this system.
 * The following should be ordered by performances, descending. */
#ifdef HAVE_EVPORT
#include "ae_evport.c"
#else
    #ifdef HAVE_EPOLL
    #include "ae_epoll.c"
    #else
        #ifdef HAVE_KQUEUE
        #include "ae_kqueue.c"
        #else
        #include "ae_select.c"
        #endif
    #endif
#endif

