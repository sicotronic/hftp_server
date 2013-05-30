#ifndef HFTP_OPS_H
#define HFTP_OPS_H
/**
 * hftp_ops.h
 * Auxiliar Functions to handle the protocol:
 *	string_t file_listing (void)
 *	int get_metadata (const string_t file, off_t * filesize);
 *	int get_slice (string_t * buffer, size_t offset, size_t len, const string_t filename);
 *	void quit_request (int sockfd);
 *
 *	for the list of errors read constants.h
 */

#include <sys/types.h>
#include "tad_string.h"

/* Returns the list of files in the directoy where the server is executed */
string_t file_listing (void);

/* Takes the file, and updates the value of filesize,
 * success: returns CODE_OK
 * error: returns FILE_NOT_FOUND
 */
int get_metadata (const string_t file, off_t * filesize);

/* Sends a part of len bytes of the file from the given offset
 * exito: returns CODE_OK
 * error: depending on the error it can return:	INTERNAL_ERROR
 *						BAD_OFFSET
 *						FILE_NOT_FOUND
*/
int get_slice (string_t * buffer, size_t offset, size_t len, const string_t filename);

/* Closes the socket file descriptor sockfd
* Precondition: (sockfd >= 0)
*/
void quit_request (int sockfd);

#endif
