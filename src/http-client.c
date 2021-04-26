

#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* index */
#include <unistd.h>
#include <ctype.h>
#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdio.h>
	#pragma comment(lib, "Ws2_32.lib")
#elif _LINUX
	#include <sys/socket.h>
#elif __FreeBSD__
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#else
/*error Platform not suppoted.*/
#endif

#include <errno.h>
#include "stringx.h" 
#include "urlparser.h" /* parseurl */
#include "utils.h" /* init_lut */
#include "http-client.h" 


struct chunk
{
	size_t size;
	char *data;
};


int create_connection(struct parsed_url *purl)
{
	int sock = 0;
	int tmpres = 0;
	struct sockaddr_in *remote;
	
	/* Create TCP socket */
	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
	    printf("Can't create TCP socket");
		return 0;
	}

	/* Set remote->sin_addr.s_addr */
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
  	tmpres = inet_pton(AF_INET, purl->ip, (void *)(&(remote->sin_addr.s_addr)));
  	if( tmpres < 0)
  	{
	    	printf("Can't set remote->sin_addr.s_addr");
	    	return 0;
  	}
	else if(tmpres == 0)
  	{
		printf("Not a valid IP");
    		return 0;
  	}
	remote->sin_port = htons(atoi(purl->port));

	/* Connect */
	if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0)
	{
		printf("Could not connect");
		return 0;
	}
	return sock;
}


/*
	get_header_value(struct http_headers* header, const char* key)
	
	looks for a header named 'key' and returns its value.
	
	if no such header found, returns NULL
*/

char* get_header_value(struct http_headers* headers, const char* key)
{
	int i = 0;
	while(NULL != headers->headers[i])
	{
		char *current = headers->headers[i];
		char* key_pos = strstr(current, key);
		if(NULL == key_pos)
		{
			++i;
			continue;
		}
		
		
		char *val_pos = index(current, ':') + 2; /* 2 for the [: ]*/
		return val_pos;	
	}
	
	
	return NULL;
	
}

/* 
	reads from the socket into a buffer until delim is encountered
	if include_delim is non-zero, the delim is included in the returned data
	
	Allocates memory for the buffer.
 */

struct chunk read_chunk(int sock)
{
	struct chunk chunk = { 0 };
	int read_bytes = 0;
	int chunk_size = 0;
	
	char *chunk_size_str = NULL;
	chunk_size_str = read_until(sock, "\r\n", 0);
	if(NULL == chunk_size_str)
	{
		chunk.data = NULL;
		return chunk;
	}
	
	chunk_size = convert_hex_to_int(chunk_size_str);
	if(-1 == chunk_size)
	{
		return chunk;
	}
	chunk.size = (size_t)chunk_size;
	
/*	read_until allocates memory*/
	free(chunk_size_str);

	#define MAX_CHUNK (20000)
	char *buffer = malloc(MAX_CHUNK);
	if(NULL == buffer)
	{
		perror("read_chunk");
		
		chunk.data = NULL;
		return chunk;
	}
	
	buffer = read_all(sock, chunk_size);
	
	/* verify crlf after chunk */
	char crlf[3];
	int n = read(sock, &crlf, 2);
	if(2 != n)
	{
		perror("read_chunk: read crlf");

		chunk.data = NULL;
		return chunk;
	}
	crlf[2] = 0;
	
	if(strcmp(crlf, "\r\n"))
	{
		fprintf(stderr, "read_chunk: didn't get crlf after chunk\n");
		
		chunk.data = NULL;
		return chunk;
	}	
	
	chunk.data = buffer;
	return chunk;

}

char *read_chunked_data(int sock)
{
	char *buffer = NULL;
	size_t total_size = 0;
	struct chunk current_chunk = { 0 };
		
	do
	{
		/* read current chunk */
		current_chunk = read_chunk(sock);
		if(NULL == current_chunk.data)
		{
			return NULL;
		}
		
		buffer = realloc(buffer, total_size + current_chunk.size);
		/* allocate new buffer */
		if(NULL == buffer)
		{
			perror("read_chunked_data: realloc");
			
			return NULL;
		}
		
		buffer[100] = 'a';
		/* copy the current chunk to end of buffer */
		if(NULL == memcpy(buffer + total_size, current_chunk.data, current_chunk.size))
		{
			perror("read_chunked_data: memcpy");
			
			return NULL;
		}
		
		total_size += current_chunk.size;
		
	} 
	while(current_chunk.size != 0);
	
	return buffer;
}

void *read_stream(int sock)
{
/*	char *buffer = NULL;*/
/*	size_t total_size = 0;*/
/*	int should_run = 1;*/
/*	*/
/*	while(should_run)*/
/*	{*/
/*		char *chunk = read_chunk(sock);*/
/*		if(NULL == chunk)*/
/*		{*/
/*			return NULL;*/
/*		}*/
		
		/* reallocated buffer */
/*		if(NULL == realloc(buffer, total_size + chunk_size))*/
/*		{*/
/*		*/
/*		}*/
/*	*/
/*	}*/
}

char *read_unchunked_data(int sock, struct http_headers *headers)
{
	/* find content length */
	char *buffer = NULL;
	int length = 0;
	char *length_str = get_header_value(headers, "Content-Length");
	if(NULL == length_str)
	{
		return NULL;
	}
	
	length = atoi(length_str);
	buffer = read_all(sock, length);
	
	return buffer;
}

char *read_data(int sock, struct http_headers *headers, int is_stream)
{
	char *buffer = NULL;
	/* check if data is chunked */
	char *transfer_encoding_header = get_header_value(headers, "Transfer-Encoding");
	if(NULL == transfer_encoding_header || strcmp("chunked", transfer_encoding_header)) /* if no Transfer-Encoding header, or non-equal to chunked*/
	{
		buffer = read_unchunked_data(sock, headers);
	}
	else if(!strcmp("chunked", transfer_encoding_header))
	{
		if(is_stream)
		{
			read_stream(sock);
		}
		else
		{
			buffer = read_chunked_data(sock);
		}
	}
	
	return buffer;
}

/*
	reads the response headers from the socket
*/

struct http_headers *read_response_headers(int sock)
{
	
	struct http_headers *headers = (struct http_headers*)malloc(sizeof(*headers));
	int i = 0;
	do
	{
		headers->headers[i] = read_until(sock, "\r\n", 0);
		++i;
	}
	while(strlen(headers->headers[i - 1]));
	
	headers->headers[i - 1] = NULL;
	
	return headers;
}

/*
	reads the status line of the response and returns the char* to it
*/
char *read_status_line(int sock)
{
	int ret_val = 0;
	/* get_until allocates memory */
	char *status_line = read_until(sock, "\r\n", 0);
	if(NULL == status_line)
	{
		return NULL;
	}
	
	return status_line;
}

/*
	gets the response status from the status_line and converts it to int
*/
int get_response_status(const char *status_line)
{
	int status_code = 0;
	char *pos = index(status_line, ' ');
	char* status_text = ++pos;
	
	while(' ' != *pos)
	{
		++pos;
	}
	*pos = 0;
	
	status_code = atoi(status_text);
	
	return status_code; 
}

struct http_response* http_req(char *req_headers, struct parsed_url *purl, int is_stream)
{
	char *data = NULL;
	int tmpres = 0;
	int sock = 0;
	int is_chunked = 0;
	int response_status = 0;
	char* response_data = NULL;
	struct http_headers* response_headers = NULL;
	
	/* Parse url */
	if(purl == NULL || req_headers == NULL)
	{
		printf("Unable to parse url");
		return NULL;
	}
	
	/* Allocate memory for response struct */
	struct http_response *hresp = (struct http_response*)malloc(sizeof(struct http_response));
	if(hresp == NULL)
	{
		printf("Unable to allocate memory for htmlcontent.");
		return NULL;
	}
	
	hresp->request_headers = req_headers;
	hresp->request_uri = purl;
	
	hresp->body = NULL;
	hresp->response_headers = NULL;
	hresp->status_code = NULL;
	hresp->status_text = NULL;
	
	/* open TCP socket */
	if(!(sock = create_connection(purl))) 
	{
		printf("Creating connection failed !\n");
		
		return NULL;
	}
	
	/* Send request headers to server */
	int sent = 0;
	while(sent < strlen(req_headers))
	{
	     tmpres = send(sock, req_headers + sent, strlen(req_headers) - sent, 0);
		if(tmpres == -1)
		{
			printf("Can't send headers");
			return NULL;
		}
		sent += tmpres;
	}
	
	/* read the status line */
	hresp->status_text = read_status_line(sock);
	
	/* extract the status code as integer */
	response_status = get_response_status(hresp->status_text);
	/* check response status code */
	if(response_status < 200 || response_status > 299)
	{
		printf("ERROR: Line: %d response status: %d\n", __LINE__, response_status);
		
		return NULL;
	} 
	hresp->status_code_int = response_status;
	
	/* Read headers into response headers */
	response_headers = read_response_headers(sock);
	if(NULL == response_headers)
	{
		printf("Couldn't read headers\n");
		
		return NULL;
	}
	
	hresp->response_headers = response_headers;
	
	data = read_data(sock, hresp->response_headers, is_stream);
	if(NULL == data)
	{
		return NULL;
	}
	
	hresp->body = data;
	/*
	
	char buffer[BUFSIZ];
	size_t recived_len = 0;
	int is_first_buffer = 1;
	
	while((recived_len = recv(sock, buffer, BUFSIZ-1, 0)) > 0)
	{	
		process_buffer(buffer, recived_len, is_first_buffer);
		is_first_buffer = 0;
	}
	if (recived_len < 0)
    	{
		free(http_headers);
		#ifdef _WIN32
			closesocket(sock);
		#else
			close(sock);
		#endif
	   printf("Unabel to recieve");
		return NULL;
    	}

	/* Reallocate response */
	/*response = (char*)realloc(response, strlen(response) + 1);
*/
	/* Close socket */
	#ifdef _WIN32
		closesocket(sock);
	#else
		close(sock);
	#endif

	

	/* Return response */
	return hresp;

}

/*
	Makes a HTTP GET request to the given url
*/
struct http_response* http_get(char *url, char *custom_headers, int is_stream)
{
	/* Parse url */
	struct parsed_url *purl = parse_url(url);
	if(purl == NULL)
	{
		printf("Unable to parse url");
		return NULL;
	}

	/* DEBUG */
	
	/*fprintf(stderr, "host: %s\nip: %s\nport: %s\npath: %s\n", purl->host, purl->ip, purl->port, purl->path);
	*/
	/* END DEBUG*/
	
	/* Declare variable */
	char *http_headers = (char*)malloc(1024);

	/* Build query/headers */
	if(purl->path != NULL)
	{
		if(purl->query != NULL)
		{
			sprintf(http_headers, "GET /%s?%s HTTP/1.1\r\nHost:%s\r\nConnection:keep-alive\r\n", purl->path, purl->query, purl->host);
		}
		else
		{
			sprintf(http_headers, "GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:keep-alive\r\n", purl->path, purl->host);
		}
	}
	else
	{
		if(purl->query != NULL)
		{
			sprintf(http_headers, "GET /?%s HTTP/1.1\r\nHost:%s\r\nConnection:keep-alive\r\n", purl->query, purl->host);
		}
		else
		{
			sprintf(http_headers, "GET / HTTP/1.1\r\nHost:%s\r\nConnection:keep-alive\r\n", purl->host); /*Connection:close\r\n*/
		}
	}


	/* Add custom headers, and close */
	if(custom_headers != NULL)
	{
		sprintf(http_headers, "%s%s\r\n", http_headers, custom_headers);
	}
	else
	{
		sprintf(http_headers, "%s\r\n", http_headers);
	}
	http_headers = (char*)realloc(http_headers, strlen(http_headers) + 1);

	/* Make request and return response */
	struct http_response *hresp = http_req(http_headers, purl, is_stream);

	return hresp;
}


/*
	Free memory of http_response
*/
void http_response_free(struct http_response *hresp)
{
	if(hresp != NULL)
	{
		/*if(hresp->request_uri != NULL) parsed_url_free(hresp->request_uri);*/
		if(hresp->body != NULL) free(hresp->body);
		if(hresp->status_code != NULL) free(hresp->status_code);
		if(hresp->status_text != NULL) free(hresp->status_text);
		if(hresp->request_headers != NULL) free(hresp->request_headers);
		if(hresp->response_headers != NULL) free(hresp->response_headers);
		free(hresp);
	}
}


