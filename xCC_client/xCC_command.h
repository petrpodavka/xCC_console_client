#pragma once
#include <vector>
#include <string>
#include <stdexcept>

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

enum class Operation {
	unknown = -1,
	get_outside_temp = 1001
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
	template<typename T> void addData(T x);
	template<typename T> void readData(T* x);
};