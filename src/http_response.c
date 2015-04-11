#include <stdio.h>
#include <stdlib.h>

#include "http_response.h"

//ein Teil des Responses ist bei uns erstmal statisch
#define HTTP_RESPONSE_TPL "HTTP/1.0 %i %s\r\nConnection: close\r\n\r\n%s\r\n"

/**
 * Erzeugt einen HTTP-Response-String auf dem Heap mit den Daten der übergebenen Struktur
 */
char *build_http_response_str(const response_t *response) {
	char *response_str;
	if(-1 == asprintf(&response_str, HTTP_RESPONSE_TPL, response->status_code, response->status_message, response->content)) {
		//asprintf macht implizit ein malloc
		perror("Memory allocation failed in build_http_response_str()");
		exit(EXIT_FAILURE);
	}
	return response_str;
}

void free_response_t(response_t *response) {
	if(response != NULL) {
		if(response->content != NULL) { free(response->content); response->content = NULL; }
		if(response->status_message != NULL) { free(response->status_message); response->status_message = NULL; }
		free(response);
	}
}
