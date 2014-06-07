#ifndef ATOMIC_H
#define ATOMIC_H

typedef struct {
        int counter;
} atomic_t;

#define ATOMIC_INIT(i) { (i) }

#define atomic_read(v) (*(volatile int *)&(v)->counter)

#define atomic_set(v, i) ((v)->counter = (i))

static __inline__ void atomic_add(int i, atomic_t * v)
{
	int temp;

	do {
	    __asm__ __volatile__(
	    "       .set    mips3                           \n"
	    "       ll      %0, %1          # atomic_add    \n"
	    "       addu    %0, %2                          \n"
	    "       sc      %0, %1                          \n"
	    "       .set    mips0                           \n"
	    : "=&r" (temp), "+m" (v->counter)
	    : "Ir" (i));
	} while (!temp);
}

static __inline__ void atomic_sub(int i, atomic_t * v)
{
    int temp;

    do {
            __asm__ __volatile__(
            "       .set    mips3                           \n"
            "       ll      %0, %1          # atomic_sub    \n"
            "       subu    %0, %2                          \n"
            "       sc      %0, %1                          \n"
            "       .set    mips0                           \n"
            : "=&r" (temp), "+m" (v->counter)
            : "Ir" (i));
    } while (!temp);
}

static __inline__ int atomic_add_return(int i, atomic_t * v)
{
	int result, temp;

	do {
        __asm__ __volatile__(
        "       .set    mips3                           \n"
        "       ll      %1, %2  # atomic_add_return     \n"
        "       addu    %0, %1, %3                      \n"
        "       sc      %0, %2                          \n"
        "       .set    mips0                           \n"
        : "=&r" (result), "=&r" (temp), "+m" (v->counter)
        : "Ir" (i));
	} while (!result);

	result = temp + i;

	return result;
}

static __inline__ int atomic_sub_return(int i, atomic_t * v)
{
	int result, temp;

    do {
        __asm__ __volatile__(
        "       .set    mips3                           \n"
        "       ll      %1, %2  # atomic_add_return     \n"
        "       subu    %0, %1, %3                      \n"
        "       sc      %0, %2                          \n"
        "       .set    mips0                           \n"
        : "=&r" (result), "=&r" (temp), "+m" (v->counter)
        : "Ir" (i));
    } while (!result);

    result = temp - i;

    return result;
}

#define atomic_dec_return(v) atomic_sub_return(1, (v))
#define atomic_inc_return(v) atomic_add_return(1, (v))
#define atomic_dec(v) atomic_sub(1, (v))
#define atomic_inc(v) atomic_add(1, (v))

#endif