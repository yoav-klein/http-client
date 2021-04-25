

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h> /* NULL */


#define ASCII_SIZE 127

void init_lut();

char *read_until(int sock, const char *delim, int include_delim);

char *read_all(int sock, size_t length);

unsigned int convert_hex_to_int(unsigned char* hex_str);


#endif // __UTILS_H__