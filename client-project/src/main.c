/*
 * main.c
 *
 * TCP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP client
 * portable across Windows, Linux and macOS.
 */
#include <ctype.h>

#if defined WIN32
#include <winsock.h>
#include <getopt.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement client logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	char *serverAddress = SERVER_ADDRESS;
	int serverPort = SERVER_PORT;
	char *typeCity = NULL;
	int opt;

	//check witch tags are used in the command line
	while ((opt = getopt(argc, argv, "s:p:r:")) != -1) {
	    switch (opt) {
	        case 's':
	        	serverAddress = optarg;
	            break;
	        case 'p':
	        	serverPort = atoi(optarg);
	            break;
	        case 'r':
	        	typeCity = optarg;
	            break;
	        default:
	            fprintf(stderr, "Usage: %s [-s server] [-p port] -r \"type city\"\n", argv[0]);
	            exit(EXIT_FAILURE);
	    }
	}

	if (typeCity == NULL) {
	        fprintf(stderr, "Errore: E' richiesto il parametro -r \"type city\"\n");
	        exit(EXIT_FAILURE);
	    }

	// TODO: Create socket
	// my_socket = socket(...);

	int my_socket;

	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);


	if(my_socket < 0){
		errorhandler("Creazione della socket non riuscita.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	// TODO: Configure server address
	struct sockaddr_in server_addr;

	//this function clean the space of memory where server_addr is stored to avoid errors caused by old data
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	server_addr.sin_addr.s_addr = inet_addr(serverAddress);

	// TODO: Connect to server
	if (connect(my_socket,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		errorhandler( "Failed to connect.\n" );
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	// TODO: Implement communication logic
	weather_request_t request;

	char buffer[100];

	//avoiding problem of losing data
	strcpy(buffer, typeCity);
	char *type = strtok(buffer, " ");
	char *city = strtok(NULL, " ");

	//check if data insert into -r "..." are correct
	if (type == NULL || city == NULL) {
	    fprintf(stderr, "Formato non valido per -r. Usa: -r \"t city\"\n");
	    closesocket(my_socket);
	    clearwinsock();
	    return -1;
	}

	memset(&request, 0, sizeof(request));

	//the first char is the type
	request.type = type[0];

	strncpy(request.city, city, sizeof(request.city) - 1);

	// INVIARE DATI AL SERVER
	if (send(my_socket, (char *)&request, sizeof(request), 0) != sizeof(request)) {
	    errorhandler("send() sent a different number of bytes than expected");
	    closesocket(my_socket);
	    clearwinsock();
	    return -1;
	}

	// RICEVERE DATI DAL SERVER
	weather_response_t response;

	int data_received = recv(my_socket, (char*)&response, sizeof(response), 0);

	if (data_received <= 0) {
		errorhandler("recv() failed or connection closed prematurely.\n");
		closesocket(my_socket);
		return -1;
	}


    printf("\n--- Risposta dal server ---\n");
    printf("Ricevuto risultato dal server ip %s.", serverAddress);
	switch (response.status) {
	    case SUCCESS:
	    	printf(" %s:", request.city);
	        switch (tolower((unsigned char)response.type)) {
				case TYPE_TEMP:
					printf(" Temperatura = %.1f°C ", response.value);
					break;
				case TYPE_HUM:
					printf(" Umidità = %.1f%%", response.value);
					break;
				case TYPE_WIND:
					printf(" Velocità del vento = %.1fkm/h", response.value);
					break;
				case TYPE_PRESS:
					printf(" Pressione = %.1f hPa", response.value);
					break;
	        }
	        break;

	    case ERROR_CITY:
	        printf(" Città non disponibile\n");
	        break;

	    case ERROR_REQUEST:
	        printf(" Richiesta non valida.\n");
	        break;

	    default:
	        printf(" Errore sconosciuto dal server.\n");
	        break;
	}

	// TODO: Close socket
	closesocket(my_socket);
	clearwinsock();
	printf("\n");
	#if defined WIN32
	system("pause");
	#endif
	return(0);
} // main end
