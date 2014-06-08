#include <stdio.h>
#include <stdlib.h>
#include "atomic.h"

#define MAX_VALUE 500
#define MULT_PROC

atomic_t finish = {0};
atomic_t proc = {0};
atomic_t * input;
atomic_t * count;
volatile int mutex = 0, n = 0, loaded = 0;

void atomic_countingsort(atomic_t * input, int start, int end)
{
	int i;

	for (i = start; i <= end; i++)
	{
		atomic_inc(&count[atomic_read(&input[i])]);
	}

	return;
}

int main(int argc, char *argv[]){
	int proc_num;
	FILE* f;
	int i, tmp;
	char cwd[1024];

	proc_num = atomic_inc_return(&proc);

	if (proc_num == 1)
	{
		f = fopen("../../sw/counting_sort/input.txt", "r");
		fscanf(f, "%d", &n);
		input = (atomic_t*) malloc(sizeof(atomic_t) * n);
		count = (atomic_t*) malloc(sizeof(atomic_t) * (MAX_VALUE+1));
		for (i = 0; i < n; i++)
		{
			fscanf(f, "%d", &tmp);
			atomic_set(&input[i], tmp);
			atomic_set(&count[i], 0);
		}
		atomic_set(&count[n], 0);
		fclose(f);
		loaded = 1;
	}
	while(!loaded);

	tmp = n/8;

	#ifndef MULT_PROC
	if (proc_num == 1)
	{
		atomic_countingsort(input, 0, n-1);
	}

	#else
	if (proc_num != 8)
	{
		atomic_countingsort(input, (proc_num-1)*tmp, proc_num*tmp-1);
	}
	else
	{
		atomic_countingsort(input, (proc_num-1)*tmp, n-1);
	}
	#endif

	atomic_inc(&finish);

	if (atomic_read(&finish) == 8)
	{
		tmp = 0;
		for (i = 0; i <= MAX_VALUE; i++)
		{
			while (atomic_read(&count[i]))
			{
				atomic_set(&input[tmp++], i);
				printf ("%d\n", i);
				atomic_dec(&count[i]);
			}
		}
	}

	return 0; // Never executed, just for compatibility
}