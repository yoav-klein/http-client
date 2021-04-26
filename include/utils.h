

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h> /* NULL */


#define ASCII_SIZE 127

void init_lut();


/*
	read_until
	
	reads from the socket until delim is found,
	allocates memory for the read content and returns it.
	
	if include_delim is non-zero, the delimiter is included in the returned buffer
	
	if all the content read contains only the delimiter, and include_delim is 0, 
	the function returns NULL 
	
	sock - socket
*/
char *read_until(int sock, const char *delim, int include_delim);

char *read_all(int sock, char *buffer, size_t length);

long int convert_hex_to_int(const unsigned char* hex_str);


#endif // __UTILS_H__
