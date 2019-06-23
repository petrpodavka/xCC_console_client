#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <map>

enum class DataType {
	BOOL,
	BYTE,
	EKV,
	INT,
	NPROG,
	REAL,
	TIME,
	TIMETABLE,
	USINT
};

const std::map<DataType, std::string> DataTypeString = {
	{ DataType::BOOL, "BOOL" },
	{ DataType::BYTE, "BYTE" },
	{ DataType::EKV, "EKV" },
	{ DataType::INT, "INT" },
	{ DataType::NPROG, "NPROG" },
	{ DataType::REAL, "REAL" },
	{ DataType::TIME, "TIME" },
	{ DataType::TIMETABLE, "TIMETABLE" },
	{ DataType::USINT, "USINT" }
};

enum Operation {
	unknown = -1,
	get_outside_temp = 1001
};

const std::map<Operation, std::pair<std::vector<DataType>, std::vector<DataType>>> OperationConfigMap = {
	{Operation::get_outside_temp, { 
		std::vector<DataType>{},
		std::vector<DataType>{DataType::REAL} 
	}}
};

/**
 * Command consists of operation and data. Data can be empty when operation doesn't require any.
 */
class Command
{
protected:
	Operation m_operation;
	std::vector<uint8_t> m_data;

public:
	Command() : m_operation(Operation::unknown) {};
	Command(Operation operation) : m_operation(operation) {};
	Command(Operation operation, std::vector<uint8_t> data) : m_operation(operation), m_data(data) {};

	Operation getOperation();
	std::vector<uint8_t> getData();

	void addData(std::string valueString, DataType type);
	template<typename T> void addData(T x);

	std::string readData(DataType type);
	template<typename T> void readData(T* x);
};