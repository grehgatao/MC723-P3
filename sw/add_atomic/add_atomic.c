#include <stdio.h>
#include "atomic.h"

atomic_t count = {0};
atomic_t finish = {0};

int main(int argc, char *argv[]){
	int i;

	for(i=0;i<10000;i++)
	{
		atomic_inc(&count);
	}

	atomic_inc(&finish);

	//Se
	if (atomic_read(&finish) == 8)
	{
		printf ("%d\n",atomic_read(&count));
	}

	return 0; // Never executed, just for compatibility
}