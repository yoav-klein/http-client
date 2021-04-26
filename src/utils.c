

#include <sys/types.h> /* recv */
#include <sys/socket.h> /* recv */
#include <string.h> /* strlen */
#include <stdlib.h> /* malloc */
#include <unistd.h> /* read */
#include "utils.h"

unsigned char ascii_to_digit[ASCII_SIZE] = { 0 };

long int convert_hex_to_int(const unsigned char* hex_str)
{
	unsigned int result = 0;
	
	init_lut();
	
	while(*hex_str)
	{
		if(!((*hex_str <= '9' && *hex_str >= '0') || (*hex_str <= 'f' && *hex_str >= 'a')))
		{
			return -1;
		}
		result = (result << 4) | ascii_to_digit[*hex_str];	
		
		++hex_str;
	}
	
	return result;
}

char *read_until(int sock, const char *delim, int include_delim)
{
	char *ret_buf = NULL;
	char buffer[BUFSIZ];
	int read_bytes = 0;
	int found = 0;
	int counter = 0;
	
	while(!found)
	{
		
		int n = recv(sock, (buffer + read_bytes), 1, 0);
		if(-1 == n)
		{
			perror("read_until");
			
			return NULL;
		}
		if(buffer[read_bytes] == delim[counter])
		{
			++counter;
			if(strlen(delim) == counter)
			{
				found = 1;
			}
		}
		else
		{
			counter = 0;
		}
		++read_bytes;	
	}
	buffer[read_bytes] = 0;
	
	if(!include_delim)
	{
		buffer[read_bytes - strlen(delim)] = 0;
	}
	
	if(strlen(buffer) && 1 == found)
	{
		ret_buf = (char*)malloc(strlen(buffer) + 1);
		strcpy(ret_buf, buffer);	
		return ret_buf;
	}
	
	return NULL;
}

char *read_all(int sock, char *buffer, size_t length)
{
	int read_bytes = 0;
	
	if(NULL == buffer)
	{
		return NULL;
	}
	
	while(read_bytes < length)
	{
		int n = read(sock, (buffer + read_bytes), (length - read_bytes));
		if(-1 == n)
		{
			perror("read_all");
			
			return NULL;
		}
		read_bytes += n;
	}
	
	
	return buffer;
}

void init_lut()
{
	ascii_to_digit['0'] = 0;
	ascii_to_digit['1'] = 1;
	ascii_to_digit['2'] = 2;
	ascii_to_digit['3'] = 3;
	ascii_to_digit['4'] = 4;
	ascii_to_digit['5'] = 5;
	ascii_to_digit['6'] = 6;
	ascii_to_digit['7'] = 7;
	ascii_to_digit['8'] = 8;
	ascii_to_digit['9'] = 9;
	ascii_to_digit['a'] = 10;
	ascii_to_digit['b'] = 11;
	ascii_to_digit['c'] = 12;
	ascii_to_digit['d'] = 13;
	ascii_to_digit['e'] = 14;
	ascii_to_digit['f'] = 15;
}


