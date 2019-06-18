#pragma once
#include <vector>
#include "xCC_command.h"

constexpr auto START_BYTE = (char) 0x01;
constexpr auto END_BYTE = (char) 0xFD;

/**
 * Message represents protocol frame. Message is defined by its command, which is defined by its operation and data. Those can be composed into frame and vice versa.
 *
 * +---------+-------+-----+---------+---------+-----+--------+----+--------+-------+-------+
 * | Byte    | 0     | 1   | 2       | 3       | 4   | 5      | .. | n      | n + 1 | n + 2 |
 * +---------+-------+-----+---------+---------+-----+--------+----+--------+-------+-------+
 * | Meaning | START | UID | CMD LSB | CMD MSB | LEN | DATA_0 | .. | DATA_n | CRC   | END   |
 * +---------+-------+-----+---------+---------+-----+--------+----+--------+-------+-------+
 */
class Message {
public:
	Command m_command;
	std::vector<uint8_t> m_frame;

	Message(std::string commandString);
	Message(Operation operation);
	Message(Command command);
	Message(std::vector<uint8_t> frame);

	void composeFrame();
	void decomposeFrame();

private:
	char computeCRC(std::vector<uint8_t> frame);
};