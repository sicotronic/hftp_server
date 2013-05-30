#define _GNU_SOURCE
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "list_s.h"
#include "parser.h"
#include "constants.h"
#include "common.h"

int parse_string (const string_t client_string, list_s lista) {
	string_t almost_done = NULL;
	string_t done = NULL;
	FILE * strf = NULL;

	int result = CODE_OK;

	assert (client_string != NULL && lista != NULL);
	strf = fmemopen (client_string, strlen(client_string) + 1, "r");
	if (strf != NULL) {
		Lexer * l = lexer_new (strf);

		lexer_next_to (l, "\n");
		almost_done = lexer_item (l);
		if (strncmp (client_string, almost_done, strlen (client_string) - 2) != 0)
			return BAD_EOL;
		lexer_destroy (l);
		fclose (strf);
		
		if (strlen (almost_done) > 1 && almost_done != NULL) {
			strf = fmemopen (almost_done, strlen(almost_done), "r");
			if (strf != NULL) {
				Lexer * j = lexer_new (strf);
				lexer_next_to (j, "\r");
				done = lexer_item (j);
				lexer_destroy (j);
				fclose (strf);

				if (strlen (done) > 1 && done != NULL) {
					strf = fmemopen (done, strlen(done), "r");
					if (strf){
						string_t valid_elem = NULL;
						Lexer * p = lexer_new (strf);
	
						while (!lexer_is_off (p)) {
							lexer_next (p, SUPERALPHANUMERIC);
							valid_elem = lexer_item (p);
							list_adjoin_back (lista, valid_elem);
							lexer_skip (p, BLANK);
						}
						lexer_destroy (p); p = NULL;
						fclose (strf);
					}
					else {
						fprintf (stderr, "fmemopen");
						result = INTERNAL_ERROR;
					}
				}
				else {
					if (done != NULL)
						result = STRING_VOID;
					else
						result = INTERNAL_ERROR;
				}
			}
			else {
				fprintf (stderr, "fmemopen");
				result = INTERNAL_ERROR;
			}
		}
		else {
			if (almost_done != NULL)
				result = STRING_VOID;
			else
				result = INTERNAL_ERROR;
		}
		if(almost_done != NULL) {
			free (almost_done); almost_done = NULL;
		}
		if(done != NULL) {
			free (done); done = NULL;
		}
	}
	else {
		fprintf (stderr, "fmemopen");
		result = INTERNAL_ERROR;
	}

	return result;
}
