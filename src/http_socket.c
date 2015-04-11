#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
#include "http_response.h"

//Nicht öffentliche Vorwärtsdeklarationen (static -> Funktionen sind nur innerhalb dieser Datei bekannt)
static void handle_client_connection(const int client_sock);
static char *receive_request(const int client_sock);
static void send_response(const char *response, const int client_sock);

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
	server_addr.sin_port = htons(80);
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
	char *response_str = NULL; //todo: zu response_t ändern

	//Request empfangen
	request_str = receive_request(client_sock); //heap pointer
	printf("Request: '%s'\n\n", request_str);

    //Request parsen todo: da kümmert sich Imma und liefert ein struct mit den Informationen
	parsed_request = parse_request(request_str); //heap pointer

	//Request verarbeiten
	//in request_processor.c
	//response_t *response = process_request(const parsed_request_t * parsed_request); //todo: implementieren
		//die Funktion soll tun:
		//prüfen ob request vorhanden und vollständig
		//prüfen ob requestmethode unterstützt (nur GET)
		//je nach URL:
		//Variante 1) url=/utc/plain oder /utc/json oder /utc/html -> liefern aktuelle UTC Zeit -> in time_ressource.c
		//Variante 2) Datei von Festplatte ausliefern (Dateien aus ./htdocs/<angeforderte Ressource>) -> in file_ressource.c (readfile.c verwenden)
		//		-> Datei existiert nicht -> Fehler 404
		//		-> Datei existiert -> Response mit Status 200 und Inhalt der Datei
		//liefert responsestruct mit: http-status-code, http-status-message, content (text, zeit, etc.)

//    sample response
//    HTTP/1.1 200 OK
//    Server: nginx/1.7.4
//    Date: Mon, 30 Mar 2015 13:34:10 GMT
//    Content-Type: text/html; charset=UTF-8
//    Connection: close
	//Test-Response, das wird später nicht mehr benötigt
//    time_t now;
//    time(&now);
//	char str_utc_time[sizeof "0000-00-00T00:00:00Z"];
//    strftime(str_utc_time, sizeof str_utc_time, "%FT%TZ", gmtime(&now));
//	char *response_tpl = "HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n<html style=\"font-size:30pt;\">Hello World<br/>%s</html>\r\n";
//	response = realloc(response, strlen(str_utc_time) + strlen(response_tpl) + 1);
//	sprintf(response, response_tpl, str_utc_time);
//	printf("%s\n",response);

	//Ein Beispielresponse...
	response_data = malloc(sizeof(response_t));
	response_data->status_code = 200;
	char *status_message = "OK";
	response_data->status_message = malloc(strlen(status_message)+1);
	strcpy(response_data->status_message, status_message);
	char *content = "Hello World";
	response_data->content = malloc(strlen(content)+1);
	strcpy(response_data->content, content);

	//Reponse String bauen und senden
	response_str = build_http_response_str(response_data); //heap pointer
	send_response(response_str, client_sock);

    // Verbindung schliessen
    if (-1 == close(client_sock)) { perror("Error closing client socket.\n"); }
    else { printf("Connection closed.\n"); }

    //dyn. Speicher freigeben
    //todo: für die structs entsprechende Funktionen zum Freigeben anlegen (da wo sie definiert sind) und hier dann aufrufen
    free_parsed_request(parsed_request); parsed_request = NULL;
    free(request_str); request_str = NULL;
    free_response_t(response_data); response_data = NULL;
    free(response_str); response_str = NULL;
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
 * Sendet eine Nachricht an den Client
 */
void send_response(const char *response, const int client_sock) {
    printf("Sending '%s' (%i) to Client\n", response, (int)strlen(response));
	if (send(client_sock, response, strlen(response), 0) != strlen(response)) {
		perror("Failed to send data to client\n");
	}
}
