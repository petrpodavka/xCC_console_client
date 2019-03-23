#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

#define DEFAULT_BUFLEN 512

int xCC_connect(std::string address, int port);
int xCC_send(std::vector<uint8_t> data);
int xCC_receive(std::vector<uint8_t>& data);
void xCC_shutdown_and_cleanup();