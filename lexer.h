#ifndef _LEXER_H
#define _LEXER_H

#include <stdio.h>   /* Para tener FILE */
#include <stdbool.h> /* Para tener bool */
#include "tad_string.h"

/*** Constantes ***/
#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER "abcdefghijklmnopqrstuvwxyz"
#define DIGIT "0123456789"
#define BLANK "\t\n\r "
#define SPECIAL "._-%$#@!?+*&/()"
#define ALPHA UPPER LOWER
#define ALPHANUMERIC UPPER LOWER DIGIT
#define SUPERALPHANUMERIC UPPER LOWER DIGIT SPECIAL

typedef struct _lexer_t Lexer;

/*** Inicialization and destruction ***/
Lexer *lexer_new (FILE *f);
	/* New lexer, reading from `f'
	 *     f is open
	 * ENSURES:
	 *     Result != NULL
	 *     ! lexer_is_started (Result)
	 */

void lexer_destroy (Lexer *self);
	/* Destroys `self', freeing associated resources. Does not close the file
	 * REQUIRES:
	 *     self != NULL
	 */

/*** Consults ***/

/* Note: all is side-effect free */
string_t lexer_item (Lexer *self);
	/* Last string read by `self'. The returned pointer must be freed
	 * by the one who receives it. 
	 * REQUIRES:
	 *     self != NULL
	 *     !lexer_is_off (self)
	 *     lexer_is_started (self)
	 * ENSURES:
	 *     Result != NULL
	 */

bool lexer_is_started (const Lexer *self);
	/* `self' started reading the file?
	 * REQUIRES: self != NULL
	 */

bool lexer_is_off (const Lexer *self);
	/* `self' finished reading the file?
	 * REQUIRES: self != NULL
	 */

/*** Operations ***/

void lexer_next (Lexer *self, const string_t charset);
	/* Read with `self' characters that belong to `charset', as many as
	 *  posible. Let the result accesible in lexer_item (self)
	 * REQUIRES:
	 *     self != NULL
	 *     charset != NULL
	 * ENSURES:
	 *     lexer_is_started (self)
	 *     lexer_is_off (self) || 
	 *     (lexer_item (self) != NULL && 
	 *       all the characters in lexer_item (self) are in charset)
	 */

void lexer_next_to (Lexer *self, const string_t charset);
	/* Read with `self' characters that do not belong to `charset', as many as
	 *  posible. Let the result accesible in lexer_item (self)
	 * REQUIRES:
	 *     self != NULL
	 *     charset != NULL
	 * ENSURES:
	 *     lexer_is_started (self)
	 *     lexer_is_off (self) || 
	 *     (lexer_item (self) != NULL && 
	 *       all the characters in lexer_item (self) are not in charset)
	 */

void lexer_next_char (Lexer *self, const string_t charset);
	/* Reads with `self' until 1 character that belongs to `charset'. 
	 * Let the result accesible in lexer_item (self).
	 * REQUIRES:
	 *     self != NULL
	 *     charset != NULL
	 * ENSURES:
	 *     lexer_is_started (self)
	 *     lexer_is_off (self) || 
	 *     (lexer_item (self) != NULL && 
	 *       strlen(lexer_item(self))<=1 &&
	 *       the first character in lexer_item (self) is in charset)
	 */

/* Extra operations */
void lexer_skip (Lexer *self, const string_t charset);
	/* Read with `self' characters that belong to `charset', as many as
	 *  posible. The read characters are lost.
	 * REQUIRES: 
	 *     self != NULL
	 *     charset != NULL
	 * ENSURES:
	 *     lexer_is_started (self)
	 *     lexer_is_off (self) || strcmp (lexer_item (self), "") == 0
	 */

void lexer_skip_to (Lexer *self, const string_t charset);
	/* Read with `self' characters that do not belong to `charset', as many as
	 *  posible. The read characters are lost.
	 * REQUIRES: 
	 *     self != NULL
	 *     charset != NULL
	 * ENSURES:
	 *     lexer_is_started (self)
	 *     lexer_is_off (self) || strcmp (lexer_item (self), "") == 0
	 */

#endif
