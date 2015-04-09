/*
 * my_callbacks.h
 *
 *  Created on: 08.04.2015
 *      Author: Imma
 */

#ifndef MY_CALLBACKS_H_
#define MY_CALLBACKS_H_

#include <stdlib.h>
#include <string.h>

#include "http_parser.h"

typedef struct parsed_request_t
{
		char *type;
		char *request_url;
		char *host;
} parsed_request_t;

parsed_request_t parsed_request;

/**
 * Request parsen und geparste Informationen in die zurückgegebene Struktur legen
 */
parsed_request_t *parse_request(char* request);

static int message_begin_cb(http_parser *p);

int request_url_cb(http_parser *p, const char *buf, size_t len);

int header_field_cb(http_parser *p, const char *buf, size_t len);

int header_value_cb(http_parser *p, const char *buf, size_t len);

int headers_complete_cb(http_parser *p);

int request_body_cb(http_parser *p, const char *buf, size_t len);

int message_complete_cb(http_parser *p);

#endif /* MY_CALLBACKS_H_ */
