
#include <stdio.h> /* printf */
#include "http-client.h" /* http_get*/


int main()
{
	int i = 0;
	struct http_response *response = NULL;
	
	response = http_get("https://www.google.co.il", "User-Agent: curl/7.68.0\r\n", 0);
	
	fprintf(stderr, "status line: %s\n", response->status_text);
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
