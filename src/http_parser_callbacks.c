/*
 * http_parser_callback.c
 *
 *  Created on: 09.04.2015
 *      Author: Imma
 */

#include "http_parser_callbacks.h"
#include "http_request.h"

parsed_request_t *parsed_request = NULL;


//aktuellen Header auswerten
#define MAX_HEADER_FIELD_LENGTH 30

typedef enum header_field{
	host, accept, general
} header_field;

void set_current_header(char *header_str);

header_field current_header;


http_parser_settings get_http_parser_callbacks()
{
	http_parser_settings settings =
	{ .on_message_begin = message_begin_cb, .on_header_field =
					header_field_cb, .on_header_value = header_value_cb, .on_url =
					request_url_cb, .on_status = 0, .on_body = request_body_cb,
					.on_headers_complete = headers_complete_cb,
					.on_message_complete = message_complete_cb };
	return settings;
}

static int message_begin_cb(http_parser *p)
{
	return 0;
}

int request_url_cb(http_parser *p, const char *buf, size_t len)
{
	char *request_url = malloc(len + 1);

	if(NULL == request_url)
	{
		perror("Memory allocation for \"request_url\" failed");
		exit(EXIT_FAILURE);
	}

	strncpy(request_url, buf, len);
	request_url[len] = '\0';

	parsed_request->request_url = request_url;

	return 0;
}

int header_field_cb(http_parser *p, const char *buf, size_t len)
{
	char header_str[MAX_HEADER_FIELD_LENGTH];
	//char[len+1] erzeugt immer char[5] --> feste Größe (malloc vermeiden)
	memset(header_str, 0, MAX_HEADER_FIELD_LENGTH);
	strncpy(header_str, buf, len);

	set_current_header(header_str);

	return 0;
}

int header_value_cb(http_parser *p, const char *buf, size_t len)
{
	if(current_header == accept)
	{
		char *content_types = malloc(len+1);

		if(NULL == content_types)
		{
			perror("Memory allocation for \"accepted_content_types\" failed");
			exit(EXIT_FAILURE);
		}

		strncpy(content_types, buf, len);
		content_types[len] = '\0';

		parsed_request->accepted_content_types = content_types;
	}

	return 0;
}

int headers_complete_cb(http_parser *p)
{
	return 0;
}

int request_body_cb(http_parser *p, const char *buf, size_t len)
{
	return 0;
}

int message_complete_cb(http_parser *p)
{
	return 0;
}

void set_current_header(char *header_str)
{
	if(strcmp("Accept", header_str) == 0)
	{
		current_header = accept;
		return;
	}
	else if(strcmp("Host", header_str) == 0)
	{
		current_header = host;
		return;
	}//...
	else
	{
		current_header = general;
	}
}

