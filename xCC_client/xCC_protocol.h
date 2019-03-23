#pragma once
#include <vector>
#include "xCC_command.h"

#define START_BYTE (char) 0x01
#define END_BYTE (char) 0xFD

/**
 * Will compose protocol frame with given command and data
 *
 * +---------+-------+-----+---------+---------+-----+--------+----+--------+-------+-------+
 * | Byte    | 0     | 1   | 2       | 3       | 4   | 5      | .. | n      | n + 1 | n + 2 |
 * +---------+-------+-----+---------+---------+-----+--------+----+--------+-------+-------+
 * | Meaning | START | UID | CMD LSB | CMD MSB | LEN | DATA_0 | .. | DATA_n | CRC   | END   |
 * +---------+-------+-----+---------+---------+-----+--------+----+--------+-------+-------+
 **/
std::vector<uint8_t> compose_message(Command cmd);

std::vector<uint8_t> decompose_message(std::vector<uint8_t> message);