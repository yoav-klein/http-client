
#include <unistd.h> /* write */
#include <stdlib.h> /* exit */
#include <stdio.h> /* printf */
#include "http-client.h" /* http_get*/


void get(char *url, char *custom_headers)
{
	int i = 0;
	struct http_response *response = NULL;
	
	response = http_get(url, custom_headers);
	
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
}

void stream(char *url, char *custom_headers)
{
	struct http_handle *handle = init_connection(url, custom_headers);
	
	struct chunk chunk = { 0 };
	do
	{
		chunk = read_chunk(handle->sock);
		fprintf(stderr, "chunk size: %ld\n", chunk.size);
		
		int written_bytes = 0;
		while(written_bytes < chunk.size)
		{
			int n = write(1, chunk.data + written_bytes, chunk.size - written_bytes);
			written_bytes += n; 
		}
	}
	while(chunk.size > 0);
	
	close_connection(handle);

}

int main(int argc, char **argv)
{
	if(NULL == argv[1])
	{
		fprintf(stderr, "Usage: http-client.out <URL>\n");
		
		exit(0);
	}
	
	/*get(argv[1], NULL);
	*/
	
	stream(argv[1], NULL);
	
	return 0;
}
