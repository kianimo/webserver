#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_response.h"

/**
 * Erzeugt eine HTTP-Response-Datenstruktur auf dem Heap
 */
response_t *create_http_response_t(const http_status_code status, const char *content) {
	char *status_message;
	response_t *response = NULL;

	response = malloc(sizeof(response_t));
	if(NULL == response) { perror("Memory allocation failed"); exit(EXIT_FAILURE); }
	response->status_code = status;

	switch(status) {
		case HTTP_STATUS_OK: status_message = "OK"; break;
		case HTTP_STATUS_BAD_REQUEST: status_message = "BAD REQUEST"; break;
		case HTTP_STATUS_NOT_FOUND: status_message = "NOT FOUND"; break;
		case HTTP_STATUS_INTERNAL_ERROR: status_message = "INTERNAL ERROR"; break;
		case HTTP_STATUS_NOT_IMPLEMENTED: status_message = "NOT IMPLEMENTED"; break;
		default: status_message = "UNKNOWN STATUS CODE";
	}
	response->status_message = malloc(strlen(status_message)+1);
	if(NULL == response->status_message) { perror("Memory allocation failed"); exit(EXIT_FAILURE); }
	strcpy(response->status_message, status_message);

	response->content = malloc(strlen(content)+1);
	if(NULL == response->content) { perror("Memory allocation failed"); exit(EXIT_FAILURE); }
	strcpy(response->content, content);

	return response;
}

/**
 * Erzeugt einen HTTP-Response-String auf dem Heap mit den Daten der übergebenen Struktur
 */
char *build_http_response_str(const response_t *response) {
	const char * const HTTP_RESPONSE_TPL = "HTTP/1.0 %i %s\r\nConnection: close\r\n\r\n%s\r\n"; //das ist bei uns erstmal Konstant
	char *response_str;
	if(-1 == asprintf(&response_str, HTTP_RESPONSE_TPL, response->status_code, response->status_message, response->content)) {
		//asprintf macht implizit ein malloc
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	return response_str;
}

/**
 * Gibt Heap-Speicher der übergebenen Datenstruktur frei
 */
void free_response_t(response_t *response) {
	if(response != NULL) {
		if(response->content != NULL) { free(response->content); response->content = NULL; }
		if(response->status_message != NULL) { free(response->status_message); response->status_message = NULL; }
		free(response);
	}
}
