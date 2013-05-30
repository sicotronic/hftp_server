#ifndef HFTP_SERVER_H
#define HFTP_SERVER_H
/**
 * server.h
 * HFTP server internal functions
 */

#include <stdbool.h>
#include "tad_string.h"
/*Initialize the server in the given port
 *success: returns the socket file descriptor
 *error: returns SERVER_ERR
 */
int server_init (int port);

/*Accepts a conection in the socket sockpd
 *success: returns the socket file descriptor of the accepted conection
 *error: returns SERVER_ERR
 */
int accept_client (int sockfd);

/* Sends the message msg of size L through the socket sockfd
 *success: returns true
 *error: returns false
 */
bool send_to_client (int sockfd, const void * msg, size_t L);

/*Reads the file descriptor fd and updates the string client_request
 *exito: returns CODE_OK
 *error: returns CLOSE_CON
 */
int read_line (int fd, string_t * client_request);

/* Closes the socket file descriptor sockfd
 *Precondition: (sockfd >= 0)
*/
void server_quit (int clientfd);

#endif
