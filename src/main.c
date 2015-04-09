#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_socket.h"

int main(void) {
	//serversocket
	return do_socket();
	//socketverbindung aufbauen
	//daten aus verbindung lesen und in puffer speichern bis puffer voll oder \r\n\r\n empfangen wurde
	//request parsen -> bei Fehler 501
	//wenn ungültiger anfragetyp nicht implementiert -> 501
	//wenn url ungültig -> 501
	//datei zu url suchen
	//wenn nicht vorhanden 404
	//wenn nicht lesbar/keine berechtigung -> 40x
	//datei einlesen und in netzwerkverbindung ausgeben
	//verbindung schließen

	//mime types
	//utc zeiten
	//threads
}

