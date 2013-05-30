#include <stdlib.h>
#include <unistd.h>
#include "mem_fun.h"

void * req_mem (size_t len, size_t type_size, unsigned int maxtries) {
	void * ret = NULL;
	unsigned int tries = 0;
	
	while (ret == NULL && tries <= maxtries) {
			ret = calloc (len, type_size);
			sleep (tries);
			tries++;
	}
	return ret;
}

void * more_mem (void * ptr, size_t new_size, unsigned int maxtries) {
	void * new = NULL;
	unsigned int tries = 0;
	
	while (new == NULL && tries <= maxtries) {
			new = realloc (ptr, new_size);
			sleep (tries);
			tries++;
	}
	return new;
}
