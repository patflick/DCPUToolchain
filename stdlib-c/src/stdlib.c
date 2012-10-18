/**
 *
 *	File:		stdlib.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Standard Library
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Provides standard library functions.
 *
 **/

#include "stdlib.h"
int _stubapi_exit(int code);

// String conversion:
int atoi(const char* str)
{
	// TODO
	return 0;
}

// Pseudo-random sequence generation:
int rand()
{
	return 4;	// Chosen by fair dice roll.
	// Guaranteed to be random.
}

void srand(unsigned int seed)
{
	// TODO
}

// Dynamic memory management:
void* calloc(size_t num, size_t size)
{
	return malloc(num * size);
}

void free(void* ptr)
{
	__asm
	{
		.IMPORT free
		SET A, <ptr>
		JSR free
	}
}

void* malloc(size_t size)
{
	void* result;
	void** store = &result;
	__asm
	{
		.IMPORT malloc
		SET A, <size>
		JSR malloc
		SET <store>, A
	}
	return result;
}

void* realloc(void* ptr, size_t size)
{
	size_t i;
	void* mem = malloc(size);
	// DANGER: Later on realloc() will be provided by the OS.  For
	// now we know that the standard library's malloc has the length
	// 2 words behind the pointer, but this is an assumption that
	// will not work across kernels!
	for (i = 0; i < * (ptr - 2); i++)
		mem[i] = ptr[i];
	free(ptr);
	return mem;
}

// Environment:
void abort()
{
    _stubapi_exit(-1);
    // Process is terminated by kernel after this
    // point.  No further code is executed.
}

// Searching and sorting:
void* bsearch(const void* key, const void* base, size_t num, size_t size, int (*comparator)(const void* a, const void* b))
{
	// TODO
	return NULL;
}

void qsort(void* base, size_t num, size_t size, int (*comparator)(const void* a, const void* b))
{
	// TODO
}

// Integer arithmethics:
int abs(int n)
{
	if (n < 0)
		return -n;
	else
		return n;
}

struct div_s div(int numerator, int denominator)
{
	struct div_s result;
	result.quot = numerator / denominator;
	result.rem = numerator % denominator;
	return result;
}

