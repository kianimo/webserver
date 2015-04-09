/*
 * http_request.h
 *
 *  Created on: 09.04.2015
 *      Author: Imma
 */

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <stdlib.h>
#include "http_parser.h"

typedef struct parsed_request_t
{
		enum http_method method;
		char *request_url;
		//char *host;
} parsed_request_t;

extern parsed_request_t *parsed_request;

/**
 * Request parsen und geparste Informationen in die zurückgegebene Struktur legen
 */
parsed_request_t *parse_request(char* request);

/**
 * free die Request-Struktur und ihre Inhalte
 */
void free_parsed_request(parsed_request_t *parsed_request);

#endif /* HTTP_REQUEST_H_ */
