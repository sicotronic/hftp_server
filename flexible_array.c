#include <assert.h>
#include "flexible_array.h"
#include "mem_fun.h"

struct _farray_t {
	void *ptr;
	size_t size;
};

/*** Inicialization and destruction ***/

FlexibleArray *flexible_array_new (const size_t size) {
	/* New flexible array of `size' bytes */
	FlexibleArray *result = NULL;
	/* Preconditions */
	/* assert (size >= 0); */ /* Commented: fails in the compilers where size_t is unsigned */
	
	result = req_mem (1,sizeof(*result),5);
	assert (result != NULL);
	result->ptr = req_mem (size,sizeof(size_t),5);
	assert (size==0 || result->ptr != NULL);
	result->size = size;
	
	return result;
}

void flexible_array_destroy (FlexibleArray *self) {
	/* Destroys the flexible array */
	/* Preconditions */
	assert (self != NULL);
	
	free (self->ptr);
	free (self);
}

void flexible_array_release (FlexibleArray *self) {
	/* Frees the structure of the array, but not the array itseld (i.e.,
	 * it does not free the flexible_array_ptr (self)). The user must free the array 
	 * afterwards
	 */
	/* Preconditions */
	assert (self != NULL);
	
	free (self);
}

/*** Operators ***/

void flexible_array_resize (FlexibleArray *self, size_t newsize) {
	/* Resizes `self' to `newsize' bytes. */
	/* Preconditions */
	assert (self != NULL);
	/* assert (newsize >= 0); */ /* Commented: fails in the compilers where size_t is unsigned */
	
	self->ptr = more_mem (self->ptr, newsize, MAXTRIES);
}

void flexible_array_make_room (FlexibleArray *self, size_t idx) {
	/* Resizes `self' if necessary in order to `idx' be a valid index
	 * (in bytes). */
	/* Preconditions */
	assert (self != NULL);
	/* assert (idx >= 0); */ /* Commented: fails in the compilers where size_t is unsigned */
	
	if (idx>=self->size) {
		if (self->size==0) self->size = 1;
		while (self->size <= idx)
			self->size = self->size*2;
		self->ptr = more_mem (self->ptr, self->size, MAXTRIES);
	}
	assert (idx<self->size);
}

void *flexible_array_ptr (const FlexibleArray *self) {
	/* Obtains the internal pointer to the array. */
	/* Preconditions */
	assert (self != NULL);
	
	return self->ptr;
}

