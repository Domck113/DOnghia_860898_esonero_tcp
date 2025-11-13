/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
 * portable across Windows, Linux and macOS.
 */
#include <ctype.h>
#include <time.h>

#if defined WIN32
#include <winsock.h>
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

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

// TODO: Implement server logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int serverPort = SERVER_PORT;
	int opt;

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
			case 'p':
				serverPort = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	//checking log
	printf("Server listening on port %d...\n", serverPort);

	int my_socket;

	// TODO: Create socket
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(my_socket < 0){
		errorhandler("Socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	// TODO: Configure server address
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// TODO: Bind socket
	if(bind(my_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		errorhandler("bind() failed. \n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	// TODO: Set socket to listen
	if(listen(my_socket, QUEUE_SIZE) < 0){
		errorhandler("listen failed. \n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//reload value every time
	srand((unsigned int)time(NULL));

	// TODO: Implement connection acceptance loop
	while (1) {

		struct sockaddr_in conn;

		int addr_len = sizeof(conn);

		int client_socket;

		printf("Waiting for a client connection. . . \n");
		printf("\n");

		client_socket = accept(my_socket, (struct sockaddr*)&conn, &addr_len);

		if(client_socket < 0){
			errorhandler("accept() failed. \n");
			continue;
		}

		printf("Client connected: %s\n", inet_ntoa(conn.sin_addr));

		weather_request_t request;
		int data_received = recv(client_socket, (char*)&request, sizeof(request), 0);

		if (data_received <= 0) {
			errorhandler("recv() failed or connection closed prematurely.\n");
			closesocket(client_socket);
			continue;
		}

		printf("Richiesta '%c %s' dal client ip %s\n", request.type, request.city, inet_ntoa(conn.sin_addr));
		printf("\n");

		weather_response_t response;

		memset(&response, 0, sizeof(response));

		response.status = SUCCESS;

		if(!checkCityPresence(request.city)){
			response.status = ERROR_CITY;
			response.type = '\0';
			response.value = 0.0;

			send(client_socket, (char*)&response, sizeof(response), 0);
			continue;
		}

		switch (tolower((unsigned char)request.type)) {
		    case TYPE_TEMP:
		    	response.value = get_temperature();
		    	break;
		    case TYPE_HUM:
		    	response.value = get_humidity();
		    	break;
		    case TYPE_WIND:
		    	response.value = get_wind();
		    	break;
		    case TYPE_PRESS:
		    	response.value = get_pressure();
		    	break;
		    default:
		    	response.status = ERROR_REQUEST;
				response.type = '\0';
				response.value = 0.0;
				send(client_socket, (char*)&response, sizeof(response), 0);
				closesocket(client_socket);
				continue;
		}
		if(response.status == SUCCESS){
			response.type = tolower((unsigned char)request.type);
			send(client_socket, (char*)&response, sizeof(response), 0);
		}

		closesocket(client_socket);
	}
	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
