#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include "hftp_ops.h"
#include "constants.h"
#include "mem_fun.h"

#define DIRLEN 255

string_t file_listing (void) {
	string_t dirname;
	string_t dir_list = NULL;
	unsigned long dirlist_len = 512;
	DIR * dir = NULL;
	struct dirent * dentry;

	dir_list = (string_t) req_mem (dirlist_len, sizeof(char), MAXTRIES);
	if(dir_list != NULL) {
		dirname = (string_t) req_mem (DIRLEN, sizeof (char), MAXTRIES);
		if (dirname != NULL) {
			dirname = getcwd (dirname, DIRLEN);
			if(dirname != NULL) {
				dir = opendir (dirname);
				if (dir) {
					struct stat current;
					int error_dentry = 0;
					while ((dentry = readdir(dir)) != NULL) {
						if (strlen (dir_list) == dirlist_len) {
							dirlist_len = dirlist_len * 2;
							dir_list = (string_t) more_mem 
							(dir_list, dirlist_len, MAXTRIES);
						}
						if ((error_dentry = stat(dentry->d_name, &current)) == 0) {
							if (S_ISDIR (current.st_mode) == 0){
								strncat (dir_list, dentry->d_name, strlen(dentry->d_name));
								strncat (dir_list, EOL, strlen(EOL));
							}
						}
					}
					closedir (dir);
				}
				else  {
					free (dir_list);
					dir_list = NULL;
					free (dirname);
					dirname = NULL;
				}
				free (dirname);
				dirname = NULL;
			}
			else {
				free (dir_list);
				dir_list = NULL;
			}
		}
		else	{
			free (dir_list);
			dir_list = NULL;
		}
	}

	return dir_list;
}

int get_metadata (const string_t filename, off_t * filesize) {
	struct stat fs;
	int error = CODE_OK;
	
	assert (filename);
	if (stat (filename, &fs) == 0)
		* filesize = fs.st_size;
	else
		error = FILE_NOT_FOUND;

	return error;
}

int get_slice (string_t * buffer, size_t offset, size_t len, const string_t filename) {
	FILE * file = NULL;
	size_t read = 0;
	off_t file_size = 0;
	int errcode = CODE_OK;
	int get_correct = CODE_OK;
	bool seek_ok = false;

	assert (filename);
	assert (*buffer == NULL); /* we must require buffer to be NULL */
	file = fopen (filename, "r");
	if (file) {
		get_correct = get_metadata (filename, &file_size);
		if (get_correct == CODE_OK) {
			if(offset >= (size_t) file_size || offset + len > (size_t) file_size) {
				errcode = BAD_OFFSET;
				fclose (file);
			}
			else {
				* buffer = (string_t) req_mem (len+1, sizeof(char), MAXTRIES);
				if (!*buffer) {
					errcode = INTERNAL_ERROR;
				}
				else {
					seek_ok = (fseek(file, (long) offset, SEEK_SET) 
							== 0);
					if (seek_ok) {
						read = fread ((void*) *buffer, sizeof (char), len, file);
						if(read < len)
							fprintf (stderr, "fread");
					}
				}
			}
		}
		else
			errcode = FILE_NOT_FOUND;
		fclose (file);
	}
	else
		errcode = FILE_NOT_FOUND;

	return errcode;
}

void quit_request (int sockfd) {
	assert (sockfd >= 0);
	close (sockfd);
}
