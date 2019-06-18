#pragma once
#ifdef _WIN32
	#include <winsock2.h>
	#include <Ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
	#include <unistd.h> /* Needed for close() */
#endif
#include <vector>
#include <string>

#define DEFAULT_BUFLEN 512

#ifndef _WIN32
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SD_SEND SHUT_WR
#endif

class ConnectException : public std::runtime_error {
public:
	ConnectException() : std::runtime_error("Failed to connect to the server or read/write") { }
};

class Connect {
protected:
	SOCKET sock;
public:
	Connect(std::string address, int port);
	~Connect();
	int Send(std::vector<uint8_t> data);
	int Receive(std::vector<uint8_t>& data);
};