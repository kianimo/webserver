#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct response_t {
	int status_code;
	char* status_message;
	char* content;
} response_t;

char *build_http_response_str(const response_t *response);
void free_response_t(response_t *response);

#endif /* HTTP_RESPONSE_H_ */
