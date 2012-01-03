#include <stdio.h>
#include <database.h>

int main (void)
{
	int i;
	
	test(&i);
	printf("Test %d\n", i);
	
	return 0;
}