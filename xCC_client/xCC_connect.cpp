#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "xCC_connect.h"
#include "xCC_protocol.h"

int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(2, 2), &wsa_data);
#else
	return 0;
#endif
}

void sockClose(SOCKET sock) {
#ifdef _WIN32
	closesocket(sock);
#else
	close(sock);
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

bool sockInvalid(SOCKET sock) {
#ifdef _WIN32
	return sock == INVALID_SOCKET;
#else
	return sock < 0;
#endif
}

int sockErrCode(int code) {
#ifdef _WIN32
	return WSAGetLastError();
#else
	return code;
#endif // _WIN32
}

#ifdef _WIN32
int sockErrCode(SOCKET sock) {
	return WSAGetLastError();
}
#endif // _WIN32

Connect::Connect(std::string address, int port)
{
	sock = INVALID_SOCKET;

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;

	// Initialize Winsock
	iResult = sockInit();
	if (iResult != 0) {
		std::cerr << "Socket initialization failed with code: " << iResult << std::endl;
		throw ConnectException();
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		std::cerr << "getaddrinfo failed with code: " << iResult << std::endl;
		sockQuit();
		throw ConnectException();
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sockInvalid(sock)) {
			std::cerr << "socket failed with code: " << sockErrCode(sock) << std::endl;
			sockQuit();
			throw ConnectException();
		}

		// Connect to server.
		iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			sockClose(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (sockInvalid(sock)) {
		sockQuit();
		throw ConnectException();
	}
}

Connect::~Connect() {
	shutdown(sock, SD_SEND);
	sockClose(sock);
	sockQuit();
}

int Connect::Send(std::vector<uint8_t> data)
{
	if (sockInvalid(sock)) {
		std::cerr << "Unable to send, invalid socket" << std::endl;
		throw ConnectException();
	}

	int iResult;
	
	iResult = send(sock, (char*) &data[0], data.size(), 0);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "send failed with code: " << sockErrCode(iResult) << std::endl;
		sockClose(sock);
		sockQuit();
		return 1;
	}
	return 0;
}

int Connect::Receive(std::vector<uint8_t>& data)
{
	if (sockInvalid(sock)) {
		std::cerr << "Unable to receive, invalid socket" << std::endl;
		throw ConnectException();
	}

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int recvlength = 0;

	data.clear();

	int result;
	do {
		result = recv(sock, recvbuf, recvbuflen, 0);

		if (result > 0) {
			recvlength += result;
			data.insert(data.end(), recvbuf, recvbuf + result);
		}
		else if (result == 0) {
			std::cerr << "Connection closed" << std::endl;
		}
		else {
			std::cerr << "recv failed with code: " << sockErrCode(result) << std::endl;
			throw ConnectException();
		}
	} while (result > 0 && recvbuf[result-1] != END_BYTE);
	
	return recvlength;
}
