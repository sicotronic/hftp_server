#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "list_s.h"
#include "parser.h"
#include "constants.h"
#include "mem_fun.h"
#include "server.h"
#include "hftp_ops.h"
#include "tad_string.h"
#include "common.h"
#include "pflog.h"

#include <time.h>

#define FOREVER_UNTIL_CRASHES true
#define P_SIZE 256

/** GLOBAL FOR PRINTING */

string_t msg_print = NULL;

/**---------------------*/

static void error_manager (int error_code, int client_sockfd, int * finished);

static void do_operations (string_t client_request, int client_sockfd, int *finished);

static bool really_send (string_t str,  int client_sockfd);

static void really_quit (list_s lista, int client_sockfd, int *finished);

static void really_get_file_listing (list_s lista, int client_sockfd, int * finished);

static void really_get_meta_data (list_s lista, int client_sockfd, int * finished);

static void really_get_slice (list_s lista, int client_sockfd, int * finished);

static bool is_num (string_t elem);

static string_t set_time (int mode);

int main (void) {
	int sockfd = -1;
	int client_sockfd = -1;
	int finished = 0;
	string_t fecha = NULL;

	pflog ("\n###############################################################\n", STANDARD);
	pflog ("\nWelcome to the HFTP server - The Home Made File Transfer Server\n", STANDARD);
	fecha = set_time (COMP_DATE);
	pflog (fecha, STANDARD);
	free (fecha);

	pflog ("\nIniciating Server...\n", STANDARD);
	sockfd = server_init (HFTP_PORT);
	if(sockfd != SERVER_ERR) {
		int counter = 1;
		while(FOREVER_UNTIL_CRASHES) {
			msg_print = (string_t) req_mem (40, sizeof (char), MAXTRIES);
			sprintf (msg_print, "\n# CONECTION NUMBER: %i \n", counter);
			pflog (msg_print, STANDARD);
			free (msg_print); msg_print = NULL;
			fecha = set_time (JUST_TIME);
			pflog (fecha, STANDARD);
			free (fecha);

			pflog ("\nWaiting for Conection...\n", STANDARD);
			client_sockfd = accept_client (sockfd);
			finished = 0;
			if (client_sockfd != SERVER_ERR) {
				string_t client_request = NULL;

				while (finished != CLOSE_CON) {
					finished = read_line (client_sockfd, &client_request);
					do_operations (client_request, client_sockfd, &finished);
					free (client_request); client_request = NULL;
				}
				if (finished != CLOSE_CON)
					server_quit (client_sockfd);
			}
			else
				error_manager (SERVER_ERR, client_sockfd, &finished);
			counter++;
		}
	}
	else
		error_manager (SERVER_ERR, client_sockfd, &finished);

	return 0;
}

static void error_manager (int error_code, int client_sockfd, int * finished) {

		assert (finished != NULL);

		switch (error_code) {

		case SERVER_ERR: {
			string_t error = NULL;
			fscanf (stderr, "%s", error);
			if (error != NULL) {
				if (strcmp (error, "socket") == 0) {
					pflog (
					"There was an error in the configuration of the socket\n", STANDARD);
					exit (1);
				}
				else if (strcmp (error, "bind") == 0) {
					pflog ("There was an error in the bind function\n", STANDARD);
					exit (1);
				}
				else if (strcmp (error, "listen") == 0)
					pflog ("There was an error while listening to the client\n", STANDARD);
				else if (strcmp (error, "accept") == 0)
					pflog ("There was an error while accepting the client\n", STANDARD);
				else if (strcmp (error, "fread") == 0)
					pflog ("There was an error while reading the file\n", STANDARD);
				free (error);
			}
		break; };
		case BAD_EOL: {
			if (send_to_client (client_sockfd, (void *) BAD_EOL_MSG EOL, 
					(size_t) (strlen(BAD_EOL_MSG) + strlen (EOL)))) {
				pflog ("A \\n character was found outside a request terminator \\r\\n\n", STANDARD);

				msg_print = (string_t) req_mem (40, sizeof (char), MAXTRIES);
				sprintf (msg_print, "Closing the conection with socket %i...\n", client_sockfd);
				pflog (msg_print, STANDARD);
				free (msg_print); msg_print = NULL;

				quit_request (client_sockfd);
				*finished = CLOSE_CON;
			}
			else 
				error_manager (BAD_EOL, client_sockfd, finished);
		break; };
		case INTERNAL_ERROR: {
			if (send_to_client (client_sockfd, (void *) INTERNAL_ERROR_MSG EOL, 
					(size_t) (strlen(INTERNAL_ERROR_MSG) + strlen (EOL))))
				
				pflog ("There was an internal error, try again\n", STANDARD);
			else
				error_manager (INTERNAL_ERROR, client_sockfd, finished);
		break; };
		case INVALID_ARGUMENTS: {
			if (send_to_client (client_sockfd, (void *) INVALID_ARGUMENTS_MSG EOL, 
					(size_t) (strlen(INVALID_ARGUMENTS_MSG) + strlen (EOL))))
				pflog (
			"The amount of arguments are not valid fot the command, or the arguments do not have the right format.\n", STANDARD);
			else
				error_manager (INVALID_ARGUMENTS, client_sockfd, finished);
		break; };
		case INVALID_COMMAND: {
			if (send_to_client (client_sockfd, (void *) INVALID_COMMAND_MSG EOL, 
					(size_t) (strlen(INVALID_COMMAND_MSG) + strlen (EOL))))
				pflog ("The command is not a valid command\n", STANDARD);
			else
				error_manager (INVALID_COMMAND, client_sockfd, finished);
		break; };
		case FILE_NOT_FOUND: {
			if (send_to_client (client_sockfd, (void *) FILE_NOT_FOUND_MSG EOL, 
					(size_t) (strlen(FILE_NOT_FOUND_MSG) + strlen (EOL))))
				pflog ("The file was not found\n", STANDARD);
			else
				error_manager (FILE_NOT_FOUND, client_sockfd, finished);
		break; };
	}
}

static bool really_send (string_t str, int clientfd) {
	bool success = false;
	
	assert (str != NULL);
	success = send_to_client (clientfd, (void *) str, (size_t) strlen (str)); /* the string must already have the EOL at the end */
	if(success)
		success = send_to_client (clientfd, (void *) EOL, (size_t) strlen (EOL));
	else
		success = send_to_client (clientfd, (void *) INTERNAL_ERROR_MSG EOL, 
				(size_t) (strlen (INTERNAL_ERROR_MSG) + strlen (EOL)));

	return success;
}

static void do_operations (string_t client_request, int client_sockfd, int *finished){
	list_s lista;
	int operation_code = CODE_OK;
	
	assert (client_request != NULL && finished != NULL && client_sockfd >= 0);
	lista = list_empty();
	operation_code = parse_string (client_request, lista);
	if (operation_code == CODE_OK) {
		string_t first_elem = NULL;
		
		first_elem = list_disjoin_first (lista);
		assert (first_elem != NULL);
		if (strcmp (first_elem, "get_file_listing") == 0)
			really_get_file_listing (lista, client_sockfd, finished);

		else if (strcmp (first_elem, "get_metadata") == 0)
			really_get_meta_data (lista, client_sockfd, finished);

		else if (strcmp (first_elem, "get_slice") == 0)
			really_get_slice (lista, client_sockfd, finished);

		else if (strcmp (first_elem, "quit") == 0)
			really_quit (lista, client_sockfd, finished);

		else {
			list_destroy (lista);
			error_manager (INVALID_COMMAND, client_sockfd, finished);
		}
		free (first_elem);
	}

	else if (operation_code == INTERNAL_ERROR) {
		list_destroy (lista);
		error_manager (INTERNAL_ERROR, client_sockfd, finished);
	}

	else if (operation_code == BAD_EOL) {
		list_destroy (lista);
		error_manager (BAD_EOL, client_sockfd, finished);
	}

	else if (operation_code == STRING_VOID)
		free (lista);

}

static void really_quit (list_s lista, int client_sockfd, int * finished) {
	pflog ("It was requested to close the server\n", STANDARD);
	if (list_isempty (lista)) {
		if (send_to_client (client_sockfd, (void *) CODE_OK_MSG EOL, 
				(size_t) (strlen (CODE_OK_MSG) + strlen (EOL)))) {

			msg_print = (string_t) req_mem (40, sizeof (char), MAXTRIES);
			sprintf (msg_print, "Closing the conection with the socket %i...\n", client_sockfd);
			pflog (msg_print, STANDARD);
			free (msg_print); msg_print = NULL;

			quit_request (client_sockfd);
			*finished = CLOSE_CON;
		}
		else {
			pflog ("There was an error while closing the server\n", STANDARD);
			error_manager (INTERNAL_ERROR, client_sockfd, finished);
		}
		list_destroy (lista);

	}
	else {
		list_destroy (lista);
		error_manager (INVALID_ARGUMENTS, client_sockfd, finished);
	}
}

static void really_get_file_listing (list_s lista, int client_sockfd, int * finished) {
	if (list_isempty (lista)) {
		string_t list_of_files = NULL;

		pflog ("It was requested to show the files in the current directory\n", STANDARD);
		if (send_to_client (client_sockfd, (void *) CODE_OK_MSG EOL, 
				(size_t) (strlen (CODE_OK_MSG) + strlen (EOL)))) {
			list_of_files = file_listing ();
			if(list_of_files != NULL) {
				if (really_send (list_of_files, client_sockfd))
					pflog ("The list of files was sent\n", STANDARD);
				else {
					pflog ("There was an error while sending the string\n", STANDARD);
					error_manager (INTERNAL_ERROR, client_sockfd, finished);
				}
				if (list_of_files) {
					free (list_of_files);
					list_of_files = NULL;
				}
			}
			else
				error_manager (INTERNAL_ERROR, client_sockfd, finished);
			list_destroy (lista);
		}
		else
			error_manager (INTERNAL_ERROR, client_sockfd, finished);
	}
	else {
		list_destroy (lista);
		error_manager (INVALID_ARGUMENTS, client_sockfd, finished);
	}
}

static void really_get_meta_data (list_s lista, int client_sockfd, int * finished) {
	if (list_cardinal (lista) == 1) {
		string_t file_name = NULL;
		string_t offset_string = NULL;
		off_t offset = 0;
		int error_code = CODE_OK;

		file_name = list_disjoin_first (lista);
		assert (file_name != NULL);

		msg_print = (string_t) req_mem ( strlen (file_name) + 50, sizeof (char), MAXTRIES);
		sprintf (msg_print, "It was requested to show the metadata of the file: %s\n", file_name);
		pflog (msg_print, STANDARD);
		free (msg_print); msg_print = NULL;

		error_code = get_metadata (file_name, &offset);
		if (error_code == CODE_OK) {
			offset_string = (string_t) req_mem ((int) offset, sizeof (char), MAXTRIES);
			if (offset_string != NULL) {
				sprintf (offset_string, "%i", (int) offset);
				if(send_to_client (client_sockfd, (void *) CODE_OK_MSG EOL, 
						(size_t) (strlen (CODE_OK_MSG) + strlen (EOL)))) {
					if (really_send (offset_string, client_sockfd))
						pflog ("The metadata was sent\n", STANDARD);
					else
						error_manager (INTERNAL_ERROR, client_sockfd, finished);
				}
				else
				error_manager (INTERNAL_ERROR, client_sockfd, finished);
			}
			else
				error_manager (INTERNAL_ERROR, client_sockfd, finished);
		}
		else
			error_manager (error_code, client_sockfd, finished);
		free (file_name);
		file_name = NULL;
		free (offset_string);
		offset_string = NULL;
		list_destroy (lista);
	}
	else {
		list_destroy (lista);
		error_manager (INVALID_ARGUMENTS, client_sockfd, finished);
	}
}

static void really_get_slice (list_s lista, int client_sockfd, int * finished) {
	if (list_cardinal (lista) == 3) {
		string_t file_name = NULL;
		string_t offset_string = NULL;
		string_t lenght = NULL;
		
		int error_code = CODE_OK;

		file_name = list_disjoin_first (lista);
		offset_string = list_disjoin_first (lista);
		lenght = list_disjoin_first (lista);

		assert (file_name != NULL && lenght != NULL && offset_string != NULL);

		if (is_num (offset_string) && is_num (lenght)) {
			string_t buffer = NULL;
			int offset = 0;
			int len = 0;

			offset = atoi (offset_string);
			len =  atoi (lenght);

			msg_print = (string_t) req_mem (strlen (file_name) + strlen (offset_string) + strlen(lenght) + 64, sizeof (char), MAXTRIES);
			sprintf (msg_print, "It was requested to send the bytes of the file %s from %i to %i\n",
				file_name, offset, len);
			pflog (msg_print, STANDARD);
			free (msg_print); msg_print = NULL;

			error_code = get_slice (&buffer, offset, len, file_name);

			if (error_code == CODE_OK) {

				if (send_to_client (client_sockfd, (void *) CODE_OK_MSG EOL,
						(size_t) (strlen (CODE_OK_MSG) + strlen (EOL)))) {
					string_t size_info = NULL;
					void * file_frag = NULL;
					int piece = P_SIZE;
					int i = 0;

					pflog ("Sending file: ", STANDARD);
					for (i = 0; i < len ; i = i + P_SIZE) {
						if (len < i + P_SIZE)
							piece = len - i;
						size_info = (string_t) req_mem 
							(strlen(offset_string) + 2, sizeof(char), MAXTRIES);
						file_frag = req_mem (piece, sizeof(char) ,MAXTRIES);
						if (size_info != NULL && file_frag != NULL) {
							sprintf (size_info, "%i ", piece);
							file_frag = memcpy (file_frag, (void *) (buffer + i),
									 (size_t) piece);
							if (send_to_client (client_sockfd, (void *) size_info,
										(size_t) strlen (size_info)) && 
							    send_to_client (client_sockfd, (void *) file_frag,
										piece) &&
							    send_to_client (client_sockfd, (void *) EOL,
										(size_t) strlen (EOL)) ) {

								pflog (".", ONLY_SCREEN);
							}
							error_manager (SERVER_ERR, client_sockfd, finished);
							free (size_info); size_info = NULL;
							free (file_frag); file_frag = NULL;
						}
						else {
							error_manager (INTERNAL_ERROR, client_sockfd, finished);
							break;
						}
					}
					if(really_send (SLICE_DONE_MSG, client_sockfd))
						pflog ("\nFile was successfuly sent\n", STANDARD);
				}
				else
					error_manager (INTERNAL_ERROR, client_sockfd, finished);
			}
			else
				error_manager (error_code, client_sockfd, finished);
				list_destroy (lista);
			free (buffer); buffer = NULL;
		}
		else {
			list_destroy (lista);
			error_manager (INVALID_ARGUMENTS, client_sockfd, finished);
		}
		free (file_name); file_name = NULL;
		free (offset_string); offset_string = NULL;
		free (lenght); lenght = NULL;
	}
	else {
		list_destroy (lista);
		error_manager (INVALID_ARGUMENTS, client_sockfd, finished);
	}
}

static bool is_num (string_t elem) {
	bool result = true;
	int length = strlen (elem);
	int i = 0;
	assert (elem != NULL);
	for (i = 0; i < length; i++) {
		result = result && 	(elem[i] == '0' ||
					elem[i] == '1' ||
					elem[i] == '2' ||
					elem[i] == '3' ||
					elem[i] == '4' ||
					elem[i] == '5' ||
					elem[i] == '6' ||
					elem[i] == '7' ||
					elem[i] == '8' ||
					elem[i] == '9');
	}
	
	return result;
}

static string_t set_time (int mode) {
	time_t now;
	size_t amount;
	string_t fecha = NULL;
	string_t format_date = "Date: %d/%m/%Y; Time: %X \n%n";
	string_t format_time = "Time: %X \n%n";
	struct tm date;

	now = time (NULL);
	date = * (localtime (&now));
	
	if (mode == COMP_DATE) {
		fecha = (string_t) req_mem (35, sizeof (char), MAXTRIES);
		amount = strftime (fecha, 35, format_date, &date);
	}
	else { /*if (mode == JUST_TIME) */
		fecha = (string_t) req_mem (35, sizeof (char), MAXTRIES);
		amount = strftime (fecha, 35, format_time, &date);
	}

	return fecha;
}
