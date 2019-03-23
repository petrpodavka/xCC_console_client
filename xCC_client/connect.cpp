#include <stdlib.h>
#include <stdio.h>
#include "connect.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

SOCKET xCC_socket;

int xCC_connect(std::string address, int port)
{
	WSADATA wsaData;
	xCC_socket = INVALID_SOCKET;

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		xCC_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (xCC_socket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(xCC_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(xCC_socket);
			xCC_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (xCC_socket == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	return 0;
}

int xCC_send(std::vector<uint8_t> data)
{
	if (xCC_socket == INVALID_SOCKET)
		return -1;

	int iResult;
	
	iResult = send(xCC_socket, (char*) &data[0], data.size(), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(xCC_socket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int xCC_receive(std::vector<uint8_t>& data)
{
	if (xCC_socket == INVALID_SOCKET)
		return -1;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int recvlength = 0;

	data.clear();

	int result;
	do {

		result = recv(xCC_socket, recvbuf, recvbuflen, 0);
		if (result > 0) {
			recvlength += result;
			data.insert(data.end(), recvbuf, recvbuf + result);
		}
		else if (result == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (result > 0 && recvbuf[result-1] != (char) 0xFD);
	
	return recvlength;
}

void xCC_shutdown_and_cleanup() 
{
	shutdown(xCC_socket, SD_SEND);
	closesocket(xCC_socket);
	WSACleanup();
}
