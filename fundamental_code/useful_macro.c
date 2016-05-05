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
