#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "mem_fun.h"
#include "server.h"
#include "constants.h"
#include "common.h"
#include "pflog.h"

int server_init (int port) {
	int sockfd = -1;
	int addrlen;
	int yes = 1;
	struct sockaddr * srv_addr;
	struct sockaddr_in sock;
	string_t msg_print = NULL;
	
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port = htons (port);
	memset (sock.sin_zero, '\0', sizeof sock.sin_zero);	
	srv_addr = (struct sockaddr *) &sock;
	
	assert (srv_addr);
	addrlen = sizeof (struct sockaddr_in);
	
	sockfd = socket (PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf (stderr, "socket");
		return SERVER_ERR;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		fprintf (stderr, "setsockopt");
		return SERVER_ERR;
	}
	assert (sockfd >= 0);
	if (bind (sockfd, srv_addr, addrlen) != 0) {
		fprintf (stderr, "bind");
		return SERVER_ERR;
	}

	msg_print = (string_t) req_mem (55, sizeof (char), MAXTRIES);
	sprintf (msg_print, "Running the server in the port %i\n", port);
	pflog (msg_print, STANDARD);
	free (msg_print); msg_print = NULL;

	return sockfd;
}

int accept_client (int sockfd) {
	int clientfd = -1;
	socklen_t addrlen;
	string_t client_addr = NULL;
	struct sockaddr_in cli_addr;
	string_t msg_print = NULL;
	
	assert (sockfd >= 0);
	addrlen = sizeof (cli_addr);
	if (listen (sockfd, MAX_CLIENTS) != 0) {
		fprintf (stderr,"listen");
		return SERVER_ERR; /* because the error is handled by main */
	}
	clientfd = accept (sockfd, (struct sockaddr *) &cli_addr, &addrlen);
	if (clientfd == -1) {
		fprintf (stderr, "accept");
	}
	else{
		client_addr = inet_ntoa (cli_addr.sin_addr);
		if (client_addr) {

			msg_print = (string_t) req_mem (strlen(client_addr) + 24, sizeof (char), MAXTRIES);
			sprintf (msg_print, "Conected with:  ( '%s' )\n", client_addr);
			pflog (msg_print, STANDARD);
			free (msg_print); msg_print = NULL;
		}
		else {
			pflog ("The address of the client could not be obtained\n", STANDARD);

			msg_print = (string_t) req_mem (40, sizeof (char), MAXTRIES);
			sprintf (msg_print, "Closing the conection with the socket %i...\n", clientfd);
			pflog (msg_print, STANDARD);
			free (msg_print); msg_print = NULL;

			server_quit (clientfd);
			pflog ("Listening again...\n", STANDARD);
			clientfd = accept_client(sockfd);
		}
	}

	return clientfd;
}

bool send_to_client (int sockfd, const void * msg, size_t L) {
	int status = 0;
	bool success = true;

	assert (sockfd >= 0);
	assert (msg != NULL);

	status = send (sockfd, msg, L, 0);
	if (status == -1) {
		fprintf (stderr, "send");
		success = false;
	} 
	else if (status < (int) L) {
		fprintf (stderr, "Transmission failed partially: from %d to %d bytes were sent\n", status, (int) L);
		success = false;
	}
	return success;
}

int read_line (int fd, string_t * client_request) {
	string_t aux = NULL;
	string_t buffer = NULL;
	size_t allocated = 0;
	size_t read = 0;
	ssize_t status = 0;
	int result = CLOSE_CON;

	assert (fd >= 0);
	buffer = (string_t)  req_mem (32, sizeof (char), MAXTRIES);
	if (buffer != NULL) {
		allocated = 32;
		status = recv (fd, buffer + read, 1, 0);
		while (buffer != NULL && status > 0 && buffer[read] != '\n') {
			read += 1;
			if (read == allocated) {
				allocated *= 2;
				aux = buffer;
				buffer = (string_t) more_mem (aux, allocated, MAXTRIES); 
			}
			if (buffer != NULL )
				status = recv (fd, buffer + read, 1, 0); 
			else
				free(aux);
		}
		* client_request = (string_t) req_mem (strlen (buffer), sizeof (char), MAXTRIES);
		if (* client_request != NULL) {
			strncpy (* client_request, buffer, strlen(buffer));
			result = CODE_OK;
		}
		else
			result = CLOSE_CON;
		if (buffer != NULL ) {
			buffer[read] = '\0';
			if (status < 0) {
				result = CLOSE_CON;
				fprintf (stderr, "read_line: recv");
			}
		}
		free (buffer);
		buffer = NULL;
	}

	return result;
}

void server_quit (int clientfd) {
	assert (clientfd >= 0);
	close (clientfd);
}
