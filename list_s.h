#ifndef LISTS_H
#define LISTS_H
/**
 * list_s
 * Is a linked list, with pointers to the first and las element,
 * On each node it contains a char *.
 * It also has a mark to explore the different elements
 */

/** implementation:
 *          _______
 *         / _     \ _      _ _      _ _      _ _
 *  list :|_| |   |_|_| -> |_|_| -> |_|_| -> | |_| -.
 *         /  \_______________________________/      v
 *        |
 *  mark |_|
*/

#include <stdbool.h>
#include "tad_string.h"

typedef struct s_node * node;
typedef struct s_list_s * list_s;

/* Creates an empty list_s */
/* If there is not enough memory returns NULL */
list_s list_empty (void);

/* Destroys a list_s*/
void list_destroy (list_s l);

/* Adds an element to the beginning of the list_s */
/* If there is not enough memory returns NULL */
void list_adjoin_front (list_s l, string_t elem);

/* Adds an element to the end of the list_s */
/* If there is not enough memory the function does nothing */
void list_adjoin_back (list_s l, string_t elem);

/* Returns the first element
 * {Pre: l != NULL} 
 * It must be freed
 * If there is not enough memory it returns NULL and the element is not taken
 */
string_t list_disjoin_first (list_s l);

/* Return the last element
 * {Pre: l != NULL}
 * It must be freed
 * If there is not enough memory it returns NULL and the element is not taken
 */
string_t list_disjoin_last (list_s l);

/* Returns the size of the list_s */
int list_cardinal (list_s l);

/* Returns True if list_s is empty */
bool list_isempty (list_s l);

/* It makes a copy of the current element pointed by the mark to a char* */
/* If there is not enough memory it returns NULL and it does nothing */
string_t list_elem (list_s l);

/* Returns a copy of the head of list_s
 * It must be freed
 * If there is not enough memory it returns NULL and it does nothing
 */
string_t list_head (list_s l);

/* Returns a copy of the tail of list_s
 * It must be freed
 * If there is not enough memory it returns NULL and it does nothing
 */
list_s list_tail (list_s l);

/* Returns a copy of l in clon
 * It must be freed
 * If there is not enough memory it returns NULL and it does nothing
 */
void list_clone (list_s l, list_s clon);

/* Concatenate the second list into the first */ 
void list_concat (list_s l, const list_s p);

/* Returns a copy of the element number num from the list, 
 * It must be freed
 * If there is not enough memory or the element number num does not exists
 * returns NULL and it does nothing
 */
string_t list_index (list_s l, int num);


/* Returns the amount of times that the string elem is in the list */
int list_amount (list_s l, string_t elem);

#endif
