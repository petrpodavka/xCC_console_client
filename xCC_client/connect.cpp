#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "connect.h"

#ifdef _WIN32
	typedef SOCKET sock;
	#define INVALID_SOCK INVALID_SOCKET
#else
	typedef int sock;
	#define INVALID_SOCK -1
	#define SOCKET_ERROR -1
	#define SD_SEND SHUT_WR
#endif

sock xCC_socket;

int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(2, 2), &wsa_data);
#else
	return 0;
#endif
}

int sockQuit(void)
{
#ifdef _WIN32
	return WSACleanup();
#else
	return 0;
#endif
}

bool sockValid(sock socket) {
#ifdef _WIN32
	return socket != INVALID_SOCK;
#else
	return socket >= 0;
#endif
}

int xCC_connect(std::string address, int port)
{
	xCC_socket = INVALID_SOCK;

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;

	// Initialize Winsock
	iResult = sockInit();
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		sockQuit();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		xCC_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (!sockValid(xCC_socket)) {
#ifdef _WIN32
			printf("socket failed with error: %ld\n", WSAGetLastError());
#else
			printf("socket failed with error: %d\n", xCC_socket);
#endif // _WIN32

			
			sockQuit();
			return 1;
		}

		// Connect to server.
		iResult = connect(xCC_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
#ifdef _WIN32
			closesocket(xCC_socket);
#else
			close(xCC_socket);
#endif
			xCC_socket = INVALID_SOCK;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (!sockValid(xCC_socket)) {
		sockQuit();
		return 1;
	}

	return 0;
}

int xCC_send(std::vector<uint8_t> data)
{
	if (!sockValid(xCC_socket))
		return -1;

	int iResult;
	
	iResult = send(xCC_socket, (char*) &data[0], data.size(), 0);
	if (iResult == SOCKET_ERROR) {
#ifdef _WIN32
		printf("send failed with error: %ld\n", WSAGetLastError());
		closesocket(xCC_socket);
#else
		printf("send failed with error: %d\n", iResult);
		close(xCC_socket);
#endif // _WIN32
		sockQuit();
		return 1;
	}
	return 0;
}

int xCC_receive(std::vector<uint8_t>& data)
{
	if (!sockValid(xCC_socket))
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
#ifdef _WIN32
			printf("recv failed with error: %d\n", WSAGetLastError());
#else
			printf("recv failed with error: %d\n", result);
#endif

	} while (result > 0 && recvbuf[result-1] != (char) 0xFD);
	
	return recvlength;
}

void xCC_shutdown_and_cleanup() 
{
	shutdown(xCC_socket, SD_SEND);
#ifdef _WIN32
	closesocket(xCC_socket);
#else
	close(xCC_socket);
#endif
	sockQuit();
}
