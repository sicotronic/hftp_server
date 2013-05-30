#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "flexible_array.h"
#include "lexer.h"
#include "mem_fun.h"

struct _lexer_t {
	/* Input tape. File and current character */
	FILE *input;
	int current;
	/* Token being read */
	FlexibleArray *item;
	size_t item_len;
	bool own_item; /* true when the lexer is the owner of the item reference */
	/* state indicators */
	bool is_started, is_off;
};

/*** Internals ***/

#define BASE_ALLOC 16

static void lexer_accept (Lexer *self) {
	string_t item = NULL;
	/* Preconditions */
	assert (self != NULL);
	assert (!lexer_is_off (self));
	
	flexible_array_make_room (self->item, self->item_len);
	item = flexible_array_ptr (self->item);
	item[self->item_len] = self->current;
	self->item_len = self->item_len + 1;
	/* Read */
	self->current = fgetc (self->input);
}

static void lexer_null_terminate (Lexer *self) {
	string_t item = NULL;
	/* Preconditions */
	assert (self != NULL);
	
	flexible_array_make_room (self->item, self->item_len);
	item = flexible_array_ptr (self->item);
	item[self->item_len] = '\0';
}

static void lexer_new_item (Lexer *self) {
	string_t item = NULL;
	assert (self != NULL);
	
	if (self->own_item) {
		item = flexible_array_ptr (self->item);
		item[0] = '\0';
	} else {
		flexible_array_release (self->item);
		self->item = flexible_array_new (BASE_ALLOC);
	}
	self->item_len = 0;
	self->own_item = true;
}

/*** Inicialization and destruction ***/

Lexer *lexer_new (FILE *f) {
	/* New lexer, reading from `f' */
	Lexer *result = NULL;
	/* Preconditions */
	assert (f != NULL);
	
	result = req_mem (1,sizeof(*result),5);
	assert (result != NULL);
	
	result->input      = f;
	result->current    = fgetc (f);
	result->item       = flexible_array_new (BASE_ALLOC);
	result->item_len   = 0;
	result->own_item   = true;
	result->is_off     = false;
	result->is_started = false;

	return result;
}

void lexer_destroy (Lexer *self) {
	/* Destroys `self', freeing associated resources. Does not close the file */
	assert (self != NULL);

	if (self->own_item)
		flexible_array_destroy (self->item);
	else
		flexible_array_release (self->item);
	free (self);
}

/*** Consults ***/

string_t lexer_item (Lexer *self) {
	/* Last string read by `self'. The returned pointer must be freed
	 * by the one who receives it. 
	 */
	string_t result = NULL;
	/* Preconditions */
	assert (self != NULL);
	assert (!lexer_is_off (self));
	assert (lexer_is_started (self));

	self->own_item = false;
	result = flexible_array_ptr (self->item);

	/* Postconditions*/
	assert (result != NULL);
	return result;
}

bool lexer_is_started (const Lexer *self) {
	/* `self' started reading the file? */
	/* Preconditions */
	assert (self != NULL);
	return self->is_started;
}

bool lexer_is_off (const Lexer *self) {
	/* `self' finished reading the file? */
	/* Preconditions */
	assert (self != NULL);
	return self->is_off;
}

/*** Operations ***/

void lexer_next (Lexer *self, const string_t charset) {
	/* Read with `self' characters that belong to `charset', as many as
	 *  posible. Let the result accesible in lexer_item (self)
	 */
	/* Preconditions */
	assert (self != NULL);
	assert (charset != NULL);

	lexer_new_item (self);
	while (self->current != EOF && strchr(charset, self->current)) {
		lexer_accept (self);
	}
	lexer_null_terminate (self);
	self->is_started = true;
	self->is_off = (self->current==EOF && self->item_len==0);
	/* Postconditions */
	assert (lexer_is_started (self));
}

void lexer_next_to (Lexer *self, const string_t charset) {
	/* Read with `self' characters that do not belong to `charset', as many as
	 *  posible. Let the result accesible in lexer_item (self)
	 */
	/* Preconditions */
	assert (self != NULL);
	assert (charset != NULL);

	lexer_new_item (self);
	while (self->current != EOF && !strchr(charset, self->current)) {
		lexer_accept (self);
	}
	lexer_null_terminate (self);
	self->is_started = true;
	self->is_off = (self->current==EOF && self->item_len==0);

	/* Postconditions */
	assert (lexer_is_started (self));
}

void lexer_next_char (Lexer *self, const string_t charset) {
	assert (self != NULL);
	assert (charset != NULL);

	lexer_new_item (self);
	if (self->current != EOF && strchr(charset, self->current)) {
		lexer_accept (self);
	}

	lexer_null_terminate (self);
	self->is_started = true;
	self->is_off = (self->current==EOF && self->item_len==0);

	assert (lexer_is_started (self));
}

/* Extra operations */
void lexer_skip (Lexer *self, const string_t charset) {
	lexer_next (self, charset);
	lexer_new_item (self);
}

void lexer_skip_to (Lexer *self, const string_t charset) {
	lexer_next_to (self, charset);
	lexer_new_item (self);
}

