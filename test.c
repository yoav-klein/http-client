

#include <stdio.h> /* printf */
#include <string.h> /* strcmp */
#include <stdlib.h> /* stdlib */
#include "http-client.h"


char *read_until_simulation(const char *text, const char *delim, int include_delim);

#define TEST(x) if(1 == x()) { printf("%s: SUCCESS\n", #x); } \
	else { printf("%s: FAILED\n", #x); res = 1;}



struct http_headers *get_headers()
{
	struct http_response *resp = NULL;
	int i = 0;
	
	resp = http_get("http://www.google.com", "Accept: */*\r\n");
	while(resp->response_headers->headers[i] != NULL)
	{
		++i;
	}
	
	return resp->response_headers;
}


int get_header_value_check()
{
	
	struct http_headers *headers;
	headers = get_headers();
	
	char *result = get_header_value(headers, "Server");
	
	if(!result)
	{
		printf("Couldn't find header\n");
		
		return 0;
	}	
	if(strcmp("gws", result))
	{
		printf("Doesn't match\n");
		return 0; 
	}
	
	
	return 1;
}


int get_response_headers_check()
{
	struct http_response *resp = NULL;
	int i = 0;	
	
	resp = http_get("http://www.google.com", "Accept: */*\r\n");
	while(resp->response_headers->headers[i] != NULL)
	{
		++i;
	}
	
	http_response_free(resp);
	
	return 1;
}

/*
int get_response_status_check()
{
	char *headers = "HTTP1.1 200 OK\r\nContent-Type: audio/mpeg\r\n";
	int status = get_response_status(headers);
	if(200 == status)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}*/

int convert_hex_to_int_check()
{

	unsigned int res = convert_hex_to_int("1a\n");
	printf("res: %u\n", res);
	if(res != 26)
	{
		return 0;
	}
	
	
	return 1;

}

int read_until_check()
{
	char *res = NULL;
	res = read_until_simulation("My name is yoav klein", "yo", 1);
	if(strcmp(res, "My name is yo"))
	{
		return 0;
	}
	
	res = read_until_simulation("\r\n", "\r\n", 0);
	
	
	return 1;
}

int main()
{
	
	int res = 0;
	
	TEST(get_header_value_check);
	/*TEST(get_response_status_check);*/
	TEST(get_response_headers_check);


	/*TEST(convert_hex_to_int_check);*/
	/*TEST(read_until_check);*/
	
	return res;
}
