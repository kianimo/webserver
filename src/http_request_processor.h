#ifndef HTTP_REQUEST_PROCESSOR_H_
#define HTTP_REQUEST_PROCESSOR_H_

#include "http_request.h"
#include "http_response.h"

/**
 * Verarbeitet Anfragen: Liefert zu geparsten HTTP-Requests die passenden Response-Daten
 */
response_t *process_request(const parsed_request_t * request);

#endif /* HTTP_REQUEST_PROCESSOR_H_ */
