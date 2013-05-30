#ifndef FLEXIBLE_ARRAY_H
#define FLEXIBLE_ARRAY_H
/**
 * Implementation of a flexible array
 */

#include <stdlib.h> /* Using size_t */

/* Flexible length arrays (can be modified during execution) */
typedef struct _farray_t FlexibleArray;

/*** Initialization and destruction ***/

FlexibleArray *flexible_array_new (const size_t size);
	/* New flexible array of `size' bytes
	 * REQUIRES: size >= 0
	 * ENSURES: Result != NULL
	 */

void flexible_array_destroy (FlexibleArray *self);
	/* Destroys the flexible array.
	 * REQUIRES: self != NULL
	 */

void flexible_array_release (FlexibleArray *self);
	/* Frees the structure of the array, but not the array itseld (i.e.,
	 * it does not free the flexible_array_ptr (self)). The user must free the array 
	 * afterwards
	 * REQUIRES: self != NULL
	 */

/*** Operators ***/

void flexible_array_resize (FlexibleArray *self, size_t newsize);
	/* Resizes `self' to `newsize' bytes.
	 * REQUIRES:
	 *     self != NULL
	 *     newsize >= 0
	 */

void flexible_array_make_room (FlexibleArray *self, size_t idx);
	/* Resizes `self' if necessary in order to `idx' be a valid index
	 * (in bytes).
	 * REQUIRES:
	 *     self != NULL
	 *     idx >= 0
	 */

/*** Accesories ***/

void *flexible_array_ptr (const FlexibleArray *self);
	/* Obtains the internal pointer to the array.
	 * REQUIRES: self != NULL
	 */

#endif
