#include "xCC_command.h"

Operation Command::getOperation()
{
	return m_operation;
}

std::vector<uint8_t> Command::getData()
{
	return m_data;
}

template<typename T> void Command::addData(T x)
{
	uint8_t* p = (uint8_t*)& x;

	for (int i = 0; i < sizeof(T); i++)
	{
		m_data.push_back(p[i]);
	}
}

template<typename T> void Command::readData(T* x) {
	if (m_data.size() < sizeof(T)) {
		throw std::runtime_error("Unable to read data of size " + std::to_string(sizeof(T)) + " bytes from m_data of size " + std::to_string(m_data.size()) + " bytes");
	}


	uint8_t* p = (uint8_t*)x;

	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		p[i] = m_data[i];
	}
	m_data.erase(m_data.begin(), m_data.begin() + sizeof(T));
}