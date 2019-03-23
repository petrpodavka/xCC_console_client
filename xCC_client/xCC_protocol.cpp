#include "xCC_protocol.h"

char compute_CRC(std::vector<uint8_t> message)
{
	char crc = message[0];

	for (unsigned int i = 1; i < message.size(); i++) {
		crc ^= message[i];
	}

	return crc;
}

std::vector<uint8_t> compose_message(Command cmd)
{
	auto command = cmd.get_command();
	auto data = cmd.get_data();

	if (data.size() > 255)
		throw "Cannot send data longer than 255 bytes";
	
	std::vector<uint8_t> message;
	
	message.push_back(START_BYTE);										// START_BYTE
	message.push_back(1);												// UID
	message.push_back((uint8_t)(command & 0xff));						// CMD LSB
	message.push_back((uint8_t)((command >> 8) & 0xff));				// CMD MSB
	message.push_back((uint8_t)(data.size() & 0xff));					// LEN
	std::copy(data.begin(), data.end(), std::back_inserter(message));	// DATA
	message.push_back(compute_CRC(message));							// CRC
	message.push_back(END_BYTE);										// END_BYTE

	return message;
}

std::vector<uint8_t> decompose_message(std::vector<uint8_t> message)
{
	std::vector<uint8_t> data;
	data.insert(data.end(), &message[5], &message[5 + (int) message[4]]);

	return data;
}