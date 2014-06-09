#include <stdio.h>
#include "atomic.h"
#include <math.h>

#define HARDWARE_POW_ADDR_BASE (int*)5242884
#define HARDWARE_POW_ADDR_EXPONENT (int*)5242888
#define HARDWARE_POW_ADDR_RESULT (int*)5242892

#define MULT_PROC
#define HARDWARE_POW

atomic_t count_t = {-1};
atomic_t res_t = {0};
atomic_t done = {0};
volatile int lock = 0;

#ifndef HARDWARE_POW
	#define powc(v,x) pow(v,x)
#else
	#define powc(v,x) hardware_pow(v,x)
	
	inline double hardware_pow(double base, double exponent)
	{
		int *base_addr = HARDWARE_POW_ADDR_BASE;
		int *exponent_addr = HARDWARE_POW_ADDR_EXPONENT;
		int *result_addr = HARDWARE_POW_ADDR_RESULT;
		int result;

		while(lock);
		lock = 1;
		*base_addr = (int)floor(base);
		*exponent_addr = (int)floor(exponent);
		result = *result_addr;
		lock = 0;

		return (double)result;
	}
#endif

//TODO: Implementar POW em Hardware
#ifdef MULT_PROC
int main(int argc, char * argv)
{
	double k, n, tmp;
	int i, tmp2;

	atomic_t *count = &count_t, *res = &res_t;

	while((i = atomic_inc_return(count)) < 8)
	{
		k = (double) i;

		tmp = ((120.0*powc(k,2)+151.0*k+47.0)/(512.0*powc(k,4)+1024.0*powc(k,3)+712.0*powc(k,2)+194.0*k+15.0))/powc(16.0,k);
		tmp2 = (int) round(tmp*100000000);
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

	if (atomic_inc_return(&done) != 8 )
	{
		return 0;
	}

	while((i = count++) < 8)
	{
		k = (double) i;

		tmp = ((120.0*powc(k,2)+151.0*k+47.0)/(512.0*powc(k,4)+1024.0*powc(k,3)+712.0*powc(k,2)+194.0*k+15.0))/powc(16.0,k);
		tmp2 = (int) round(tmp*100000000);
		res += tmp2;
	}

	
	printf("%d\n", res);

	return 0;
}
#endif