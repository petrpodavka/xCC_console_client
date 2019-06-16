#pragma once
#include <vector>
#include <string>
#include <stdexcept>

enum class Operation {
	get_protocol_version = 0,
	get_outside_temp = 1001
};

template<typename T> void read(T* x, std::vector<uint8_t>& buffer)
{
	if (buffer.size() < sizeof(T)) {
		throw std::runtime_error("Unable to read data of size " + std::to_string(sizeof(T)) + " bytes from buffer of size " + std::to_string(buffer.size()) + " bytes");
	}
		

	uint8_t* p = (uint8_t*)x;

	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		p[i] = buffer[i];
	}
	buffer.erase(buffer.begin(), buffer.begin() + sizeof(T));
}

/**
 * Command consists of operation and data. Data can be empty when operation doesn't require any.
 */
class Command
{
	Operation m_operation;
	std::vector<uint8_t> m_data;

public:
	Command() : m_operation(Operation::get_protocol_version) {};
	Command(Operation operation) : m_operation(operation) {};
	Command(Operation operation, std::vector<uint8_t> data) : m_operation(operation), m_data(data) {};

	Operation getOperation()
	{
		return m_operation;
	}

	std::vector<uint8_t> getData()
	{
		return m_data;
	}

	template<typename T> void addData(T x)
	{
		uint8_t* p = (uint8_t*) &x;

		for (int i = 0; i < sizeof(T); i++)
		{
			m_data.push_back(p[i]);
		}
	}

	template<typename T> void readData(T* x) {
		read(x, m_data);
	}
};