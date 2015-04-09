/*
 * http_request.c
 *
 *  Created on: 09.04.2015
 *      Author: Imma
 */

#include "http_request.h"
#include "http_parser_callbacks.h"

parsed_request_t *parse_request(char* request)
{
	//struktur in der die Callbacks ihre Daten ablegen
	parsed_request = malloc(sizeof(parsed_request));

	http_parser_settings settings = get_http_parser_callbacks();

	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);

	http_parser_execute(&parser, &settings, request, strlen(request)+1);

	parsed_request->method = parser.method;

	return NULL;
}

void free_parsed_request(parsed_request_t *parsed_request)
{
	//TODO
}


