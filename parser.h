#ifndef PARSER_H
/**
 * parser.h
 * Implements a request parser
 * Used to habdle the requests of our HFTP client
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "tad_string.h"

/* Parse the order client_string 
 * success: Updates the list with the command of the client followed by
 *	   the arguments if present, returns CODE_OK
 *
 * error: depending on the error it can return:	INTERNAL_ERROR
 *						BAD_EOL
 *						STRING_VOID
*/
int parse_string (const string_t client_string, list_s lista);

#endif /* PARSER_H */
