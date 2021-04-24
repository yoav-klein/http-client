

#include <stdio.h> /* printf */
#include <string.h> /* strcmp */
#include <stdlib.h> /* stdlib */
#include "http-client.h"

#define TEST(x) if(1 == x()) { printf("%s: SUCCESS\n", #x); } \
	else { printf("%s: FAILED\n", #x); }


int get_header_value_check()
{
	char *headers = "Content-Type: audio/mpeg\r\nConnection: Close";
	
	char *result = get_header_value(headers, "Content-Type");
	if(!result)
	{
		printf("Couldn't find header\n");
		
		return 0;
	}	
	if(strcmp("audio/mpeg", result))
	{
		printf("Doesn't match\n");
		return 0; 
	}
	
	free(result);
	return 1;
}

int get_response_headers_check()
{
	struct http_response* resp;
	resp = http_get("http://www.google.com", "Accept: */*\r\n");
	
	fprintf(stderr, "Response headers:\n%s", resp->response_headers);
	fprintf(stderr, "---- print reponse body--------\n");
	/*printf("%s", resp->body);*/
	
	http_response_free(resp);
	
	return 1;
}

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
}

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


int main()
{
	
	/*TEST(get_header_value_check);
	TEST(get_response_status_check);*/
	/*TEST(get_response_headers_check);*/


	TEST(convert_hex_to_int_check);
	
	return 0;
}
