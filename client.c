
#include <stdio.h> /* printf */
#include "http-client.h" /* http_get*/


int main()
{
	int i = 0;
	struct http_response *response = NULL;
	
	response = http_get("https://www.walla.co.il", NULL, 0);
	
	printf("status line: %s\n", response->status_text);
	printf("status: %d\n", response->status_code_int);
	
	printf("Headers:\n");
	while(NULL != response->response_headers->headers[i])
	{
		printf("%s\n", response->response_headers->headers[i]);
		++i;
	}
	
	printf("Body:\n");
	printf("%s", response->body);
	
	return 0;
}
