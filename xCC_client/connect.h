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

int xCC_connect(std::string address, int port);
int xCC_send(std::vector<uint8_t> data);
int xCC_receive(std::vector<uint8_t>& data);
void xCC_shutdown_and_cleanup();