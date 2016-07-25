#include<stdio.h>
#include<stdlib.h>

#define random(x) (rand()%x)

int main(int argc, char **argv)
{
	int x;
    for(x = 0; x < 3; x++)
    	printf("%d\n",random(100));

	return 0;
}
