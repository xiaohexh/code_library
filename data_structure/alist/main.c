#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "adlist.h"

struct person {
	int id;
	char *name;
	int age;
};

struct person persons[] = {
	{ 1001, "Chris", 23 },
	{ 1002, "Jean", 34 },
	{ 1003, "Andy", 28 },
	{ -1, NULL, -1 }
};

int main(int argc, char **argv)
{
	int i;
	list *person_list;
	list *another_list;

	/* list create */
    person_list = listCreate();

	/* list add node */
	i = 0;
	while (persons[i].id != -1) {
		listAddNodeHead(person_list, &persons[i]);
		i++;
	}

	/* list index */
	listNode *node = listIndex(person_list, 1);
	struct person *tmp = (struct person *)node->value;
	printf("index 1 info: %d\n", tmp->id);
	printf("index 1 info: %s\n", tmp->name);
	printf("index 1 info: %d\n", tmp->age);

	/* list length */
	printf("list length: %d\n", (int)(listLength(person_list)));

	/* list dup */
	another_list = listDup(person_list); 
	listNode *head = listFirst(another_list);
	tmp = (struct person *)head->value;
	printf("index 0 info: %d\n", tmp->id);
	printf("index 0 info: %s\n", tmp->name);
	printf("index 0 info: %d\n", tmp->age);

	/* list release */
	listRelease(person_list);
	listRelease(another_list);

	return 0;
}
