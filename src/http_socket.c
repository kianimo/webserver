#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#include "http_socket.h"
#include "http_request.h"
#include "http_parser_callbacks.h"
#include "http_request_processor.h"
#include "http_response.h"

//Nicht öffentliche Vorwärtsdeklarationen (static -> Funktionen sind nur innerhalb dieser Datei bekannt)
static void handle_client_connection(const int client_sock);
static char *receive_request(const int client_sock);
static void send_response(const response_t *response_data, const int client_sock);

/**
 * Erstellt einen Serversocket und nimmt bei Bedarf mehrere Verbindungen an,
 * die seriell abgearbeitet werden.
 */
int serve_http(void) {
	const int server_sock = socket(AF_INET, SOCK_STREAM, 0);

	//Serversocket erstellen
	if (-1 == server_sock) {
		perror("Error opening socket");
		exit(EXIT_FAILURE);
	}

	//IP und Port an den Serversocket binden
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(7800);
	if (-1 == bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
		perror("Error binding socket");
		exit(EXIT_FAILURE);
	}

	//Verbindungen annehmen
	printf("Waiting for connections from clients...\n");
	if (-1 == listen(server_sock, 10)) {
		perror("Error listening on socket");
		exit(EXIT_FAILURE);
	}

	while (true) {
		const int client_sock = accept(server_sock, NULL, NULL);
		if (-1 == client_sock) {
			perror("Error opening socket connection");
			exit(1);
		}
		printf("Connection established.\n");
		handle_client_connection(client_sock);
	}

	if (-1 == close(server_sock)) {
		perror("Warning: Error closing server socket.\n");
		exit(1);
	}

	return EXIT_SUCCESS;
}

/**
 * Bearbeitet die HTTP-Anfrage eines Clients
 */
static void handle_client_connection(const int client_sock) {
	char *request_str = NULL;
	parsed_request_t *parsed_request = NULL;
	response_t *response_data = NULL;

	//Request empfangen
	request_str = receive_request(client_sock); //heap pointer
	printf("Request: '%s'\n\n", request_str);

    //Request parsen
	parsed_request = parse_request(request_str); //heap pointer

	//Request verarbeiten
	response_data = process_request(parsed_request);

	//Reponse String bauen und senden
	send_response(response_data, client_sock);

    // Verbindung schliessen
    if (-1 == close(client_sock)) { perror("Error closing client socket.\n"); }
    else { printf("Connection closed.\n"); }

    //dyn. Speicher freigeben
    free_parsed_request(parsed_request); parsed_request = NULL;
    free(request_str); request_str = NULL;
    free_response_t(response_data); response_data = NULL;
}

/**
 * Einen HTTP Request aus dem Socket lesen
 */
static char *receive_request(const int client_sock) {
    static const char * const HTTP_REQUEST_TERMINATOR = "\r\n\r\n";
    const int RECV_BUFFER_SZ = 100;
    char recv_buffer[RECV_BUFFER_SZ];  // Puffer fuer empfangene und zu sendende Daten
    int bytes_received = -1; // Anzahl der empfangen Bytes
    size_t request_sz = 1;

	char *request = NULL;
    char *request_end = NULL;

    // Request empfangen
    do {
		if (-1 == (bytes_received = recv(client_sock, recv_buffer, RECV_BUFFER_SZ, 0))) {
			perror("Failed to receive additional data from client");
			exit(EXIT_FAILURE);
		}

    	//Request-Puffer vergrößern, Inhalt des Empfangspuffers anhängen und terminieren
    	size_t old_request_sz = request_sz;
    	request_sz += bytes_received;
    	request = realloc(request, request_sz);
    	memcpy(request + old_request_sz - 1, recv_buffer, bytes_received);
    	request[request_sz] = '\0';

    	//HTTP Requests enden mit einer leeren Zeile
    	request_end = strstr(request, HTTP_REQUEST_TERMINATOR);
    	if(request_end) {
    		//wir lesen keine weiteren Daten und verwerfen eventuell nachfolgende Daten (z.B. bei HTTP POST)
    		request_end[strlen(HTTP_REQUEST_TERMINATOR)] = '\0';
			printf("Request complete\n");
			break;
		}
    } while (bytes_received > 0);

    return request;
}

/**
 * Sendet die HTTP-Antwort an den Client
 */
static void send_response(const response_t *response_data, const int client_sock) {
	char *response_str = build_http_response_str(response_data); //heap pointer

    printf("Sending '%s' (%i) to Client\n", response_str, (int)strlen(response_str));
	if (send(client_sock, response_str, strlen(response_str), 0) != strlen(response_str)) {
		perror("Failed to send data to client\n");
	}

	free(response_str); response_str = NULL;
}
