#ifndef PFLOG_H
#define PFLOG_H
/**
 * pflog.h
 * New function to print messages
 */

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
		·DEBUG_STATUS can be used to print debug messages
*/

#define LOG_FILE "server.log"

/*Debug mode:	0 - OFF
 *		STANDARD - ON
 */
#define DEBUG_STATUS 0

#define ONLY_LOG 1
#define ONLY_SCREEN 2
#define STANDARD 3

/* Prints msg depending on the mode*/
void pflog (char * msg, int mode);

#endif
