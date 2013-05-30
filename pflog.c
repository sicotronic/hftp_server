#include <stdio.h>
#include "pflog.h"

/**
Usage:
	pflog (char * msg, int mode)
	Will print msg in the following way (mode):
	0 - It will ignore the print
	ONLY_LOG - Will only print it in the log file
	ONLY_SCREEN -Will only print it in the screen
	STANDAR - Will print it in the log file and the screen

	NOTE:	·msg must have the desired format and EOL
		·The log file is the given in LOG_FILE
*/

static void print_log (char * line);

void pflog (char * msg, int mode){
	if (msg) {
		switch (mode){
			case 0: {
				break;
			}
			case ONLY_LOG: {
				print_log (msg);
				break;
			}
			case ONLY_SCREEN: {
				printf("%s", msg);
				break;
			}
			default: {
				print_log (msg);
				printf("%s", msg);
				break;
			}
		}
	} 
	else {
		printf ("A message could not be printed\n");
	}
}

static void print_log (char * line){
	FILE * archivo = NULL;

	archivo = fopen (LOG_FILE, "a");
	if (archivo){
		fprintf (archivo, "%s", line);
	}
	else {
		printf ("The log file could not be opened\n");
	}
	fclose (archivo);
}
