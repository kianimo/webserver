#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

typedef enum {
	HTTP_STATUS_OK 					= 200,
	HTTP_STATUS_BAD_REQUEST			= 400,
	HTTP_STATUS_NOT_FOUND 			= 404,
	HTTP_STATUS_INTERNAL_ERROR 		= 500,
	HTTP_STATUS_NOT_IMPLEMENTED 	= 501,
} http_status_code;

typedef struct response_t {
	http_status_code status_code;
	char* status_message;
	char* content;
} response_t;


/**
 * Erzeugt eine HTTP-Response-Datenstruktur auf dem Heap
 */
response_t *create_http_response_t(const http_status_code status, const char *content);

/**
 * Erzeugt einen HTTP-Response-String auf dem Heap mit den Daten der übergebenen Struktur als Grundlage
 */
char *build_http_response_str(const response_t *response);

/**
 * Gibt Heap-Speicher der übergebenen Datenstruktur frei
 */
void free_response_t(response_t *response);

#endif /* HTTP_RESPONSE_H_ */
