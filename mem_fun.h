#ifndef MEM_FUN_H
#define MEM_FUN_H
/**
 * mem_fun.h
 * Implements new functions for memory allocation,
 * req_mem and more_mem are equivalent to calloc and realloc respectively,
 * but they use MAXTRIES to ask for memory a finite amount of times
 * if an error happens during the assignation
 */

#include <sys/types.h>

#define MAXTRIES 5

/* Allocs an amount len of memory of type type_size, in case of failure
* it repeats the process maxtries times.
*/
void * req_mem(size_t len, size_t type_size, unsigned int maxtries);

/* Modifies the memory requested by ptr to new_size, in case of failure
* it repeats the process maxtries times.
*/
void * more_mem(void * ptr, size_t new_size, unsigned int maxtries);

#endif
