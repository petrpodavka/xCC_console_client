#pragma once
#include <vector>
#include <string>

#define OUTSIDE_TEMP (short) 1001

template<typename T> void read(T* x, std::vector<uint8_t>& buffer)
{
	uint8_t* p = (uint8_t*)x;

	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		p[i] = buffer[i];
	}
	buffer.erase(buffer.begin(), buffer.begin() + sizeof(T));
}

class Command
{
	short command;
	std::vector<uint8_t> data;

public:
	Command(short cmd) : command(cmd) {};

	short get_command()
	{
		return command;
	}

	std::vector<uint8_t> get_data()
	{
		return data;
	}

	template<typename T> void add(T x)
	{
		uint8_t* p = (uint8_t*) &x;

		for (int i = 0; i < sizeof(T); i++)
		{
			data.push_back(p[i]);
		}
	}

	template<typename T> void read(T* x) {
		read(x, data);
	}
};