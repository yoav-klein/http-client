

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

struct http_headers 
{
	#define MAX_HEADERS (100)
	char *headers[MAX_HEADERS];
};

struct http_response
{
	struct parsed_url *request_uri;
	char *body;
	int status_code_int;
	char *status_text;
	char *request_headers;
	struct http_headers *response_headers;
};

struct http_handle
{
	int sock;
	struct http_response *response;
};

struct chunk
{
	size_t size;
	char *data;
};



/**** Functions ******/

/*
	init_connection
	--------------
	gets url and custom_headers from user
	opens a TCP connection with the server
	sends the HTTP request and reads the response headers
	
	NOTE: this function allocates memory for the handle struct and more
		 user must call close_connection in order to free memory
	
	returns a struct http_handle that contains the response with the headers
	and a file descriptor of the socket
	
	on error, returns NULL
*/
struct http_handle *init_connection(char *url, char *custom_headers);

/*
	close_connection
	----------------
	closes the connection
	frees the handle struct and closes the socket fd
		
*/
void close_connection(struct http_handle *handle);

/*
	read_chunk
	-----------
	reads a chunk of data from a streaming server
	
	NOTE: Allocates memory for the chunk
		 user must call free(chunk.data)
*/
struct chunk read_chunk(int sock);



/*
	http_get
	--------
	gets a url and headers from the user
	initiates a GET request from the server, and returns a struct http_response*
	that contains the response headers and body
	
	on error, returns NULL
	
	NOTE: this function allocates memory
		 user must call http_response_free(response)	
*/
struct http_response* http_get(char *url, char *custom_headers);


/*
	http_response_free
	------------------
	
	frees the memory allocated for a response

*/
void http_response_free(struct http_response *hresp);


/*
	get_header_value
	------------------
	looks for a header named 'key' and returns its value.
	
	if no such header found, returns NULL
	
	NOTE: this function DOESNT allocate any memory, so don't free anything
*/
char* get_header_value(struct http_headers*, const char* key);

/* 
	get_response_status
	-------------------
	returns an integer indicating the response status
 */
int get_response_status(const struct http_response *hresp);



/*
struct http_response* http_head(char *url, char *custom_headers);
struct http_response* http_post(char *url, char *custom_headers, char *post_data);
*/


#endif /* __HTTP_CLIENT_H__ */
