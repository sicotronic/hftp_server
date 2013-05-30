#ifndef CONSTANT_H
#define CONSTANT_H
/**
 * constants.h
 * Library used to define protocol constants 
 */

#define HFTP_PORT 19500
#define EOL "\r\n"
#define MAX_CLIENTS 10
#define SLICE_DONE_MSG "0 "
#define VALID_NUMBER '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'

#define CODE_OK_MSG "0 The task was performed successfuly."
#define BAD_EOL_MSG "100 Character \n out of the request terminator \r\n."
#define BAD_REQUEST_MSG "101 Something wrong in the request was not posible to process."
#define INTERNAL_ERROR_MSG "199 Internal failure while processing the request."
#define INVALID_COMMAND_MSG "200 The command is not a valid command."
#define INVALID_ARGUMENTS_MSG "201 Error in the sent arguments."
#define FILE_NOT_FOUND_MSG "202 File does not exist."
#define BAD_OFFSET_MSG "203 Offset does not exist for that file."


#define CODE_OK 0 
#define BAD_EOL 100
#define BAD_REQUEST 101
#define INTERNAL_ERROR 199
#define INVALID_COMMAND 200
#define INVALID_ARGUMENTS 201
#define FILE_NOT_FOUND 202
#define BAD_OFFSET 203

#endif
