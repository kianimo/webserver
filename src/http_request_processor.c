#include <string.h>
#include <stdio.h>

#include "http_request_processor.h"
#include "readfile.h"

/**
 * Liefert zu geparsten HTTP-Requests die passenden Response-Daten
 */
response_t *process_request(const parsed_request_t * request) {

	//die Funktion soll tun:
	//prüfen ob request vorhanden und vollständig
	//prüfen ob requestmethode unterstützt (nur GET)
	//je nach URL:
	//Variante 1) url=/utc/plain oder /utc/json oder /utc/html -> liefern aktuelle UTC Zeit -> in time_ressource.c
	//Variante 2) Datei von Festplatte ausliefern (Dateien aus ./htdocs/<angeforderte Ressource>) -> in file_ressource.c (readfile.c verwenden)
	//		-> Datei existiert nicht -> Fehler 404
	//		-> Datei existiert -> Response mit Status 200 und Inhalt der Datei
	//liefert responsestruct mit: http-status-code, http-status-message, content (text, zeit, etc.)

	static const char * const UTC_TIME_PATH = "/utc/";

	if(NULL == request) { return create_http_response_t(HTTP_STATUS_BAD_REQUEST, "Your request was not valid."); }
	if(HTTP_GET != request->method) { return create_http_response_t(HTTP_STATUS_NOT_IMPLEMENTED, "Sorry, your request method is not supported by this server."); }
	if(strlen(request->request_url) < 1) { return create_http_response_t(HTTP_STATUS_BAD_REQUEST, "The requests URL is not valid."); }

	//Spezielle URL -> hier soll die Zeit ausgeliefert werden
	//todo erzeugen der Zeitstrings in time_ressource.c realisieren
	// etwas in der Art:
	//    time_t now;
	//    time(&now);
	//    char str_utc_time[sizeof "0000-00-00T00:00:00Z"];
	//    strftime(str_utc_time, sizeof str_utc_time, "%FT%TZ", gmtime(&now));
	if(strstr(request->request_url, UTC_TIME_PATH) == request->request_url) {
		char *request_url_utc_type = request->request_url + strlen(UTC_TIME_PATH);
		if(strstr(request_url_utc_type, "plain/\0") == request_url_utc_type) {
			return create_http_response_t(HTTP_STATUS_OK, "plain zeit");
		}
		else if(strstr(request_url_utc_type, "html/\0") == request_url_utc_type) {
			return create_http_response_t(HTTP_STATUS_OK, "html zeit");
		}
		else if(strstr(request_url_utc_type, "json/\0") == request_url_utc_type) {
			return create_http_response_t(HTTP_STATUS_OK, "json zeit");
		}
		return create_http_response_t(HTTP_STATUS_NOT_FOUND, "Sorry, this time format is not supported by this server.");
	}

	//sonst eine Datei ausliefern
	//todo in Methode bzw. file_ressource.c auslagern, Fehlertoleranter machen und testen
	static const char * const DOCUMENT_ROOT = "./htdocs";
	static const char * const INDEX_FILE = "index.html";
	char *file_name = malloc(strlen(DOCUMENT_ROOT) + strlen(request->request_url) + strlen(INDEX_FILE) + 1);
	if(NULL == file_name) { perror("Memory allocation failed"); exit(EXIT_FAILURE); }
	strcpy(file_name, DOCUMENT_ROOT);
	strcat(file_name, request->request_url);
	if(request->request_url[strlen(request->request_url)-1]=='/') { strcat(file_name, INDEX_FILE); } //Falls nur Verzeichnis angegeben -> index.html anhängen
	char *file_content = fileToString(file_name);
	free(file_name);

	if(NULL == file_content) { return create_http_response_t(HTTP_STATUS_NOT_FOUND, "File not found."); }

	response_t *response = create_http_response_t(HTTP_STATUS_OK, file_content);
	free(file_content); //sonst memory leak, da file_content kopiert wird
	return response;
}
