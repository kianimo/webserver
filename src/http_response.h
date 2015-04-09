#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

typedef struct response_t {
	int status_code;
	char* status_message;
	char* content;
} response_t;

char *build_http_response(const response_t *response);

#endif /* HTTP_RESPONSE_H_ */
