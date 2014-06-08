#include <stdio.h>
#include "atomic.h"
#include <math.h>

#define MULT_PROC

atomic_t count_t = {-1};
atomic_t res_t = {0};
atomic_t done = {0};

//TODO: Implementar POW em Hardware
#ifdef MULT_PROC
int main(int argc, char * argv)
{
	double k, n, tmp;
	int i, tmp2;

	atomic_t *count = &count_t, *res = &res_t;

	while((i = atomic_inc_return(count)) < 5)
	{
		k = (double) i;

		tmp = ((120.0*pow(k,2)+151.0*k+47.0)/(512.0*pow(k,4)+1024.0*pow(k,3)+712.0*pow(k,2)+194.0*k+15.0))/pow(16.0,k);
		tmp2 = (int) floor(tmp*100000000);
							 //2147483647
		atomic_add(tmp2, res);
	}

	if (atomic_inc_return(&done) == 8 )
	{
		printf("%d\n", atomic_read(res));
	}

	return 0;
}
#else
int main(int argc, char * argv)
{
	double k, n, tmp;
	int i, tmp2;

	int count, res;

	while((i = count++) < 5)
	{
		k = (double) i;

		tmp = ((120.0*pow(k,2)+151.0*k+47.0)/(512.0*pow(k,4)+1024.0*pow(k,3)+712.0*pow(k,2)+194.0*k+15.0))/pow(16.0,k);
		tmp2 = (int) floor(tmp*100000000);
							 //2147483647
		res += tmp2;
	}

	if (atomic_inc_return(&done) == 8 )
	{
		printf("%d\n", res);
	}

	return 0;
}
#endif