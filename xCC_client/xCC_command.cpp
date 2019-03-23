#include <vector>
#include "xCC_command.h"

Command::Command(short cmd) : command(cmd) {}

template<typename T>
void Command::add(T x)
{
	char* p = (char*)x;

	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		data.push_back(p[i]);
	}
}

template<typename T>
void Command::read(T &x) {
	std::copy(data.begin(), data.begin() + sizeof(T), x);
	data.erase(data.begin(), data.begin() + sizeof(T));
}
/*
bool Command::readBool()
{
	char x;
	read(&x);
	return x == 0 ? false : true;
}

short Command::readShort()
{
	short x = 0;
	read(&x);
	return x;
}

float Command::readFloat()
{
	float x = 0;
	read(&x);
	return x;
}*/