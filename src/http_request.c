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

	if(parsed_request == NULL)
	{
		perror("Memory allocation for \"request_url\" failed");
		exit(EXIT_FAILURE);
	}

	http_parser_settings settings = get_http_parser_callbacks();

	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);

	http_parser_execute(&parser, &settings, request, strlen(request));

	if(parser.http_errno != 0)
	{
		fprintf(stderr, "Error while parsing. http_errno: %i\n", parser.http_errno);
	}

	parsed_request->method = parser.method;

	return parsed_request;
}

void free_parsed_request(parsed_request_t *parsed_request)
{
	if(parsed_request->request_url != NULL)
	{
		free(parsed_request->request_url);
	}

	free(parsed_request);
}


