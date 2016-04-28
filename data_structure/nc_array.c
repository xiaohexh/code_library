/*
 * twemproxy - A fast and lightweight proxy for memcached protocol.
 * Copyright (C) 2011 Twitter, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nc_array.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>


struct array *
array_create(uint32_t n, size_t size)
{
    struct array *a;

    assert(n != 0 && size != 0);

    a = malloc(sizeof(*a));
    if (a == NULL) {
        return NULL;
    }

    a->elem = malloc(n * size);
    if (a->elem == NULL) {
        free(a);
        return NULL;
    }

    a->nelem = 0;
    a->size = size;
    a->nalloc = n;

    return a;
}

void
array_destroy(struct array *a)
{
    array_deinit(a);
    free(a);
}

rstatus_t
array_init(struct array *a, uint32_t n, size_t size)
{
    assert(n != 0 && size != 0);

    a->elem = malloc(n * size);
    if (a->elem == NULL) {
        //return NC_ENOMEM;
        return -1;
    }

    a->nelem = 0;
    a->size = size;
    a->nalloc = n;

    //return NC_OK;
    return 0;
}

void
array_deinit(struct array *a)
{
    assert(a->nelem == 0);

    if (a->elem != NULL) {
        free(a->elem);
    }
}

uint32_t
array_idx(struct array *a, void *elem)
{
    uint8_t *p, *q;
    uint32_t off, idx;

    assert(elem >= a->elem);

    p = a->elem;
    q = elem;
    off = (uint32_t)(q - p);

    assert(off % (uint32_t)a->size == 0);

    idx = off / (uint32_t)a->size;

    return idx;
}

void *
array_push(struct array *a)
{
    void *elem, *new;
    size_t size;

    if (a->nelem == a->nalloc) {

        /* the array is full; allocate new array */
        size = a->size * a->nalloc;
        new = realloc(a->elem, 2 * size);
        if (new == NULL) {
            return NULL;
        }

        a->elem = new;
        a->nalloc *= 2;
    }

    elem = (uint8_t *)a->elem + a->size * a->nelem;
    a->nelem++;

    return elem;
}

void *
array_pop(struct array *a)
{
    void *elem;

    assert(a->nelem != 0);

    a->nelem--;
    elem = (uint8_t *)a->elem + a->size * a->nelem;

    return elem;
}

void *
array_get(struct array *a, uint32_t idx)
{
    void *elem;

    assert(a->nelem != 0);
    assert(idx < a->nelem);

    elem = (uint8_t *)a->elem + (a->size * idx);

    return elem;
}

void *
array_top(struct array *a)
{
    assert(a->nelem != 0);

    return array_get(a, a->nelem - 1);
}

void
array_swap(struct array *a, struct array *b)
{
    struct array tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * Sort nelem elements of the array in ascending order based on the
 * compare comparator.
 */
void
array_sort(struct array *a, array_compare_t compare)
{
    assert(a->nelem != 0);

    qsort(a->elem, a->nelem, a->size, compare);
}

/*
 * Calls the func once for each element in the array as long as func returns
 * success. On failure short-circuits and returns the error status.
 */
rstatus_t
array_each(struct array *a, array_each_t func, void *data)
{
    uint32_t i, nelem;

    assert(array_n(a) != 0);
    assert(func != NULL);

    for (i = 0, nelem = array_n(a); i < nelem; i++) {
        void *elem = array_get(a, i);
        rstatus_t status;

        status = func(elem, data);
        //if (status != NC_OK) {
        if (status != 0) {
            return status;
        }
    }

    //return NC_OK;
    return 0;
}

#if 0

/*
 * array unit test
 * this array more like stack
 * gcc -o nc_array nc_array.c
 */

rstatus_t add_something(void *elem, void *data)
{
	int *e = (int *)(elem);
	int *d = (int *)(data);

	*e += *d;
	return 0;
}

rstatus_t print_array(void *elem, void *data)
{
	int *e = (int *)elem;
	printf("%d ", *e);

	return 0;
}

int main(int argc, char **argv)
{
	struct array *a;
	uint32_t n, num;
	size_t size, i;
	int incr;

	int *elem;

	rstatus_t status;

	 n = 10;
	 size = sizeof(int);	/* int-array */

	/* create new array */
	a = array_create(n, size);
	if (a == NULL) {
		printf("array_create failed!\n");
		return 1;
	}

	/* add elem into array */
	for (i = 0; i < n; i++) {
		elem = array_push(a);
		if (elem == NULL) {
			printf("array_push failed!\n");
			array_destroy(a);
			return 1;
		}
		*elem = i + 1;
	}

	/* get idx = 3 */
	elem = array_get(a, 3);
	printf("elem at idx 3 is %d\n", *elem);

	/* get current elem num */
	num = array_n(a);

	/* array_each */
	incr = 6;
	status = array_each(a, add_something, &incr);
	if (status < 0) {
		printf("array_each failed!");
	}

	/* print each elem */
	status = array_each(a, print_array, NULL);
	printf("\n");


	/* pop all elem */
	for (i = 0; i < n; i++) {
		array_pop(a);
	}

	/* pop all elem before destroy */
	array_destroy(a);

	return 0;
}

#endif
