import socket
import sys
from constants import *

code_err = -1

class Client:
	def __init__(self, server=default_server, port=default_port):
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.status = code_err
		self.s.connect((server, port))
		self.buffer = ''
		self.connected = True

	def close(self):
		self.send('quit')
		self.status, message = self.read_response_line()
		if self.status != code_ok:
			print "Warning: Weird reply to quit command.", self.status, message
		self.s.close()

	def send(self, message):
		res = self.s.send (message + eol)
		print "Request: %s, %s bytes sent." % (message, res)
	
	def recv(self):
		data = self.s.recv(4096)
		self.buffer += data
		if len(data) == 0:
			print "Connection closed by server."
			self.connected = False

	def read_line(self):
		while not eol in self.buffer and self.connected:
			self.recv()
		response, self.buffer = self.buffer.split(eol, 1)
		return response.strip()

	def read_response_line(self):
		result = None, None
		response = self.read_line()
		if ' ' in response:
			code, message = response.split(None, 1)
			try:
				result = int(code), message
			except ValueError:
				pass
		else:
			print "Invalid response: '%s'" % response
		return result

	def read_fragment(self):
		while ' ' not in self.buffer and self.connected:
			self.recv()
		length, self.buffer = self.buffer.split (' ', 1)
		length = int(length)
		while len(self.buffer) < length and self.connected:
			self.recv()
		fragment, self.buffer = self.buffer[:length], self.buffer[length:]
		while len(self.buffer) < len(eol) and self.connected:
			self.recv()
		assert self.buffer.startswith(eol)
		self.buffer = self.buffer[len(eol):]
		return fragment

	def file_lookup(self):
		self.send ('get_file_listing')
		self.status, message = self.read_response_line()
		if self.status == code_ok:
			filename = self.read_line()
			while filename:
				print filename
				filename = self.read_line()
		else:
			print "There was a problem when querying the file listing (code=%d %s)." % (self.status, message)

	def retrieve(self, filename):
		self.send ('get_metadata %s' % filename)
		self.status, message = self.read_response_line()
		if self.status == code_ok:
			size = int(self.read_line())
			assert size >= 0
			self.send ('get_slice %s %d %d' % (filename, 0, size))
			self.status, message = self.read_response_line()
			if self.status == code_ok:
				output = open (filename, 'wb')
				fragment = self.read_fragment()
				total = 0
				while fragment:
					total += len(fragment)
					output.write(fragment)
					fragment = self.read_fragment()
				output.close()
			else:
				print "There was a problem when retreving the file %s." % filename
		elif self.status == file_not_found:
			print "Filename is incorrect."
		else:
			print "There was a problem when retreving metadata for file %s (code=%d %s)." % (filename, self.status, message)

if __name__ == '__main__':
	client = Client()
	print """\t* Welcome to HFTP - The Home Made File Transfer Program *
\t* Available files are the following:"""
	
	client.file_lookup()
	if client.status == code_ok:
		print "\t* Please enter the filename you'd like to retrieve:"
		client.retrieve(sys.stdin.readline().strip())
	
	client.close()
