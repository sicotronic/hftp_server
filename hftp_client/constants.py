# encoding: utf-8

default_server = 'localhost'
default_port = 19500

eol = '\r\n'

code_ok = 0
bad_eol = 100
bad_request = 101
internal_error = 199
invalid_command = 200
invalid_arguments = 201
file_not_found = 202
bad_offset = 203

def valid_status(s):
	return s in (code_ok, bad_eol, bad_request, internal_error,
	             invalid_command, invalid_arguments, file_not_found, bad_offset)

def fatal_status (s):
    assert valid_status (s)
    return 100 <= s < 199
