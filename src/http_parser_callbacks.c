/*
 * http_parser_callback.c
 *
 *  Created on: 09.04.2015
 *      Author: Imma
 */

#include "http_parser_callbacks.h"
#include "http_request.h"

parsed_request_t *parsed_request = NULL;

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

	strncpy(request_url, buf, len);
	request_url[len] = '\0';

	parsed_request->request_url = request_url;

	return 0;
}

int header_field_cb(http_parser *p, const char *buf, size_t len)
{
	return 0;
}

int header_value_cb(http_parser *p, const char *buf, size_t len)
{
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

