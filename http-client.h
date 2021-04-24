

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__


struct http_response
{
	struct parsed_url *request_uri;
	char *body;
	char *status_code;
	int status_code_int;
	char *status_text;
	char *request_headers;
	char *response_headers;
};

struct http_response* http_req(char *http_headers, struct parsed_url *purl);
struct http_response* http_get(char *url, char *custom_headers);
struct http_response* http_head(char *url, char *custom_headers);
struct http_response* http_post(char *url, char *custom_headers, char *post_data);

void http_response_free(struct http_response *hresp);


unsigned int convert_hex_to_int(unsigned char* hex_str);
char* get_header_value(char* headers, const char* key);
int get_response_status(char* response_headers);
char *read_until(int sock, const char *delim);

#endif /* __HTTP_CLIENT_H__ */
