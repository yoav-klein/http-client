
#include <stdlib.h> /* exit */
#include <stdio.h> /* printf */
#include "http-client.h" /* http_get*/


int main(int argc, char **argv)
{
	int i = 0;
	struct http_response *response = NULL;
	
	if(NULL == argv[1])
	{
		fprintf(stderr, "Usage: http-client.out <URL>");
		
		exit(0);
	}
	
	response = http_get(argv[1], NULL, 1);
	
	fprintf(stderr, "status: %d\n", response->status_code_int);
	
	fprintf(stderr, "Headers:\n");
	while(NULL != response->response_headers->headers[i])
	{
		fprintf(stderr, "%s\n", response->response_headers->headers[i]);
		++i;
	}
	
	fprintf(stderr, "Body:\n");
	printf("%s", response->body);
	
	http_response_free(response);
	
	return 0;
}
