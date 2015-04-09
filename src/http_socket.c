#include "http_socket.h"

void handle_client_connection(const int client_sock) {
    const int buffer_sz = 100;
    char buffer[buffer_sz];  // Puffer fuer empfangene und zu sendende Daten
    int bytes_received = -1; // Anzahl der empfangen Bytes
    char *request = NULL;
    size_t request_sz = 1;

    // Daten vom Client lesen
    if ((bytes_received = recv(client_sock, buffer, buffer_sz, 0)) == -1) {
      perror("Failed to receive initial data from client");
      exit(EXIT_FAILURE);
    }

    // Daten zum Client senden und weitere Daten vom Client lesen
    while (bytes_received > 0) {
    	size_t old_request_sz = request_sz;
    	request_sz += bytes_received;
    	request = realloc(request, request_sz);
    	memcpy(request + old_request_sz - 1, buffer, bytes_received);
    	request[request_sz] = '\0';
    	printf("Request: '%s'\n\n", request);

		if ((bytes_received = recv(client_sock, buffer, buffer_sz, 0)) == -1) {
			perror("Failed to receive additional data from client");
			exit(EXIT_FAILURE);
		}

		if(request[request_sz]=='\n') {
			printf("Request Ende");
			break;
		}
    }


//    HTTP/1.1 200 OK
//    Server: nginx/1.7.4
//    Date: Mon, 30 Mar 2015 13:34:10 GMT
//    Content-Type: text/html; charset=UTF-8
//    Transfer-Encoding: chunked
//    Connection: keep-alive
//    X-Powered-By: PHP/5.6.0


    strcpy(buffer, "HTTP/1.0 200 OK\r\n\r\nHello World\r\n");
    printf("Sending '%s' (%i) to Client\n", buffer, (int)strlen(buffer));
	if (send(client_sock, buffer, strlen(buffer), 0) != strlen(buffer)) {
		perror("Failed to send data to client\n");
		exit(EXIT_FAILURE);
	}

    // Verbindung schliessen
    if (-1 == close(client_sock)) { perror("Warning: Error closing client socket.\n"); }
    else { printf("Connection closed.\n"); }

}

// Ausbaustufe 4: Fehlerbehandlung (beliebig viele Verbindungen pro
// Programmlauf, aber nur ein Verbindung zu einem Zeitpunkt)
int do_socket(void) {
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
