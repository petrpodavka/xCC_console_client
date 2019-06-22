#include <stdexcept>
#include <iostream> 
#include <string> 
#include <sstream> 
#include "xCC_protocol.h"
#include "xCC_command.h"

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

Message::Message(std::string commandString) {
	auto parts = split(commandString, ';');

	if (parts.size() == 0)
		throw std::invalid_argument("command string cannot be empty");

	Operation operation;
	try {
		operation = static_cast<Operation>(std::stoi(parts[0]));
	}
	catch(std::exception &e) {
		auto msg = "Operation " + parts[0] + " is not valid operation (" + e.what() + ")";
		throw std::invalid_argument(msg);
	}

	m_command = Command(operation);

	auto configMap = OperationConfigMap.at(operation).first;
	for (size_t i = 1; i < parts.size(); i++) {
		auto type = configMap[i - 1];
		auto valueString = parts[i];

		m_command.addData(valueString, type);
	}
}

Message::Message(Operation operation) {
	m_command = Command(operation);
}

Message::Message(Command command) {
	m_command = command;
}

Message::Message(std::vector<uint8_t> frame) {
	m_frame = frame;
}

void Message::composeFrame() {
	if (m_command.getOperation() == Operation::unknown)
		throw std::invalid_argument("Unable to compose frame for unknown operation");

	auto operation = static_cast<short>(m_command.getOperation());
	auto data = m_command.getData();

	if (data.size() > 255)
		throw std::invalid_argument("Cannot send data longer than 255 bytes");

	m_frame.push_back(START_BYTE);										// START_BYTE
	m_frame.push_back(1);												// UID
	m_frame.push_back((uint8_t)(operation & 0xff));						// CMD LSB
	m_frame.push_back((uint8_t)((operation >> 8) & 0xff));				// CMD MSB
	m_frame.push_back((uint8_t)(data.size() & 0xff));					// LEN
	std::copy(data.begin(), data.end(), std::back_inserter(m_frame));	// DATA
	m_frame.push_back(computeCRC(m_frame));								// CRC
	m_frame.push_back(END_BYTE);										// END_BYTE
}

void Message::decomposeFrame() {
	// TODO check START_BYTE, UID, END_BYTE and CRC

	short operation = m_frame[3];
	operation <<= 8;
	operation |= m_frame[2];

	std::vector<uint8_t> data;
	data.insert(data.end(), &m_frame[5], &m_frame[5 + (int)m_frame[4]]);
	
	m_command = Command(static_cast<Operation>(operation), data);
}

char Message::computeCRC(std::vector<uint8_t> frame)
{
	char crc = frame[0];

	for (unsigned int i = 1; i < frame.size(); i++) {
		crc ^= frame[i];
	}

	return crc;
}

std::string Message::readAllData() {
	std::string response = "";
	auto configMap = OperationConfigMap.at(m_command.getOperation()).second;
	for (auto& type : configMap) {
		if (response.length() > 0)
			response += " ";
		response += m_command.readData(type);
	}

	return response;
}