#include "http_socket.h"

//Nicht öffentliche Vorwärtsdeklarationen
void handle_client_connection(const int client_sock);
char *receive_request(const int client_sock);
void send_response(const char *response, const int client_sock);

void handle_client_connection(const int client_sock) {
	char *request = NULL;
	char *response = NULL; //todo: zu response_t ändern

	//Request empfangen
	request = receive_request(client_sock);
	printf("Request: '%s'\n\n", request);

    //Request parsen todo: da kümmert sich Imma und liefert ein struct mit den Informationen

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
    time_t now;
    time(&now);
	char str_utc_time[sizeof "0000-00-00T00:00:00Z"];
    strftime(str_utc_time, sizeof str_utc_time, "%FT%TZ", gmtime(&now));
	char *response_tpl = "HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n<html style=\"font-size:30pt;\">Hello World<br/>%s</html>\r\n";
	response = realloc(response, strlen(str_utc_time) + strlen(response_tpl) + 1);
	sprintf(response, response_tpl, str_utc_time);
	printf("%s\n",response);

	//Reponse senden
	send_response(response, client_sock); //todo: reponse_t statt char verwenden!

    // Verbindung schliessen
    if (-1 == close(client_sock)) { perror("Warning: Error closing client socket.\n"); }
    else { printf("Connection closed.\n"); }

    //dyn. Speicher freigeben
    //todo: für die structs entsprechende Funktionen zum Freigeben anlegen (da wo sie definiert sind) und hier dann aufrufen
    free(request);
    free(response);
}

char *receive_request(const int client_sock) {
    const char *http_request_terminator = "\r\n\r\n";
    const size_t http_request_terminator_len = strlen(http_request_terminator);
    const int recv_buffer_sz = 100;
    char recv_buffer[recv_buffer_sz];  // Puffer fuer empfangene und zu sendende Daten
    int bytes_received = -1; // Anzahl der empfangen Bytes
    size_t request_sz = 1;

    // Request empfangen
	char *request = NULL;
    char* request_end = NULL;
    do {
		if ((bytes_received = recv(client_sock, recv_buffer, recv_buffer_sz, 0)) == -1) {
			perror("Failed to receive additional data from client");
			exit(EXIT_FAILURE);
		}

    	//Request-Puffer vergrößern, Inhalt des Empfangspuffers anhängen und terminieren
    	size_t old_request_sz = request_sz;
    	request_sz += bytes_received;
    	request = realloc(request, request_sz);
    	memcpy(request + old_request_sz - 1, recv_buffer, bytes_received);
    	request[request_sz] = '\0';
    	//printf("Request bis jetzt: '%s'\n\n", request);

    	//HTTP Requests enden mit einer leeren Zeile
    	request_end = strstr(request, http_request_terminator);
    	if(request_end) {
    		//wir lesen keine weiteren Daten und verwerfen eventuell nachfolgende Daten (z.B. bei HTTP POST)
    		request_end[http_request_terminator_len] = '\0';
			printf("Request complete\n");
			break;
		}
    } while (bytes_received > 0);

    return request;
}

void send_response(const char *response, const int client_sock) {
    printf("Sending '%s' (%i) to Client\n", response, (int)strlen(response));
	if (send(client_sock, response, strlen(response), 0) != strlen(response)) {
		perror("Failed to send data to client\n");
		exit(EXIT_FAILURE);
	}
}

// Ausbaustufe 4: Fehlerbehandlung (beliebig viele Verbindungen pro
// Programmlauf, aber nur ein Verbindung zu einem Zeitpunkt)
int serve_http(void) {
  const int server_sock = socket(AF_INET, SOCK_STREAM, 0);

  //Serversocket erstellen
  if (-1 == server_sock) {
    fprintf(stderr, "Error opening socket: %s\n", strerror(errno));
    exit(1);
  }

  //IP und Port an den Serversocket binden
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);
  server_addr.sin_port = htons(80);
  if (-1 == bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    fprintf(stderr, "Error binding socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  //Verbindungen annehmen
  printf("Waiting for connections from clients...\n");
  if (-1 == listen(server_sock, 10)) {
    // besser perror verwenden
    fprintf(stderr, "Error listening on socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  while(true) {
    const int client_sock = accept(server_sock, NULL, NULL);
    if (-1 == client_sock) {
      perror("Error opening socket.");
      exit(1);
    }
    printf("Connection established.\n");
    handle_client_connection(client_sock);

  }

  if (close(server_sock) == -1) {
    perror("Warning: Error closing server socket.\n");
  }

  return EXIT_SUCCESS;
}
