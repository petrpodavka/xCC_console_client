#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <limits>
#include "xCC_command.h"

Operation Command::getOperation()
{
	return m_operation;
}

std::vector<uint8_t> Command::getData()
{
	return m_data;
}

std::string getMessage(std::string valueString, DataType type, std::string what) {
	return "Value '" + valueString + "' not parsable as " + DataTypeString.at(type) + " (" + what + ")";
}

void Command::addData(std::string valueString, DataType type) {
	unsigned int valueUint;
	int valueInt;

	switch (type)
	{
	case DataType::BOOL:
		if (valueString == "true") {
			addData<char>(1);
			break;
		}
		else if (valueString == "false") {
			addData<char>(0);
			break;
		}
		throw std::invalid_argument(getMessage(valueString, type, "true/false"));
	case DataType::REAL:
		try {
			addData<float>(std::stof(valueString));
			break;
		}
		catch (std::exception& e) {
			throw std::invalid_argument(getMessage(valueString, type, e.what()));
		}
	case DataType::USINT:
	case DataType::BYTE:
		try {
			valueUint = std::stoul(valueString);
		}
		catch (std::exception & e) {
			throw std::invalid_argument(getMessage(valueString, type, e.what()));
		}

		if (valueUint < std::numeric_limits<unsigned char>::min() || valueUint > std::numeric_limits<unsigned char>::max())
			throw std::invalid_argument(getMessage(valueString, type, "must be 0 - 255"));

		addData<char>((char)valueUint);
		break;
	case DataType::INT:
		try {
			valueInt = std::stoi(valueString);
		}
		catch (std::exception & e) {
			throw std::invalid_argument(getMessage(valueString, type, e.what()));
		}

		if (valueInt < std::numeric_limits<short>::min() || valueInt > std::numeric_limits<short>::max())
			throw std::invalid_argument(getMessage(valueString, type, "must be -32768 - 32767"));

		addData<short>((short)valueInt);
		break;
	case DataType::TIME:
		try {
			valueInt = std::stoi(valueString);
		}
		catch (std::exception & e) {
			throw std::invalid_argument(getMessage(valueString, type, e.what()));
		}
		addData<int>(valueInt);
		break;
	case DataType::EKV:
	case DataType::NPROG:
	case DataType::TIMETABLE:
	default:
		throw std::invalid_argument("DataType " + DataTypeString.at(type) + " not implemented yet");
	}
}

template<typename T> void Command::addData(T x)
{
	uint8_t* p = (uint8_t*)& x;

	for (int i = 0; i < sizeof(T); i++)
	{
		m_data.push_back(p[i]);
	}
}

std::string Command::readData(DataType type) {
	std::stringstream stream;
	char valueChar;
	float valueFloat;
	short valueShort;
	int valueInt;

	switch (type)
	{
	case DataType::BOOL:
		readData<char>(&valueChar);

		if (valueChar == 1)
			return "true";
		else 
			return "false";
	case DataType::REAL:	
		readData<float>(&valueFloat);

		stream << std::fixed << std::setprecision(3) << valueFloat;
		return stream.str();
	case DataType::USINT:
	case DataType::BYTE:
		readData<char>(&valueChar);

		stream << (unsigned int)valueChar;
		return stream.str();
	case DataType::INT:
		readData<short>(&valueShort);

		stream << valueShort;
		return stream.str();
	case DataType::TIME:
		readData<int>(&valueInt);

		stream << valueInt;
		return stream.str();
	case DataType::EKV:
	case DataType::NPROG:
	case DataType::TIMETABLE:
	
	default:
		throw std::invalid_argument("DataType " + DataTypeString.at(type) + " not implemented yet");
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