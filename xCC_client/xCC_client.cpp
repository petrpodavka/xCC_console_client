#include <iostream>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <algorithm>
#include "xCC_connect.h"
#include "xCC_protocol.h"
#include "xCC_command.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 61000

// How to Parse Command Line Arguments in C++? [duplicate]
// https://stackoverflow.com/a/868894
class InputParser {
public:
	InputParser(int& argc, char** argv) {
		for (int i = 1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}
	/// @author iain
	const std::string& getCmdOption(const std::string & option) const {
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}
	/// @author iain
	bool cmdOptionExists(const std::string & option) const {
		return std::find(this->tokens.begin(), this->tokens.end(), option)
			!= this->tokens.end();
	}
private:
	std::vector <std::string> tokens;
};

void printBuffer(std::vector<uint8_t> buffer) {
	std::cout << "raw data (" << buffer.size() << " bytes): 0x";
	for (unsigned int i = 0; i < buffer.size(); i++)
	{
		std::cout << std::hex << std::internal << std::setfill('0') << std::setw(2) << (int)(unsigned char)buffer[i];
	}
	std::cout << std::endl;
}

int main(int argc, char** argv)
{
	InputParser input(argc, argv);

	std::string address = DEFAULT_ADDRESS;
	int port = DEFAULT_PORT;
	bool debug = false;
	std::string commandString;

	if (argc == 1 || input.cmdOptionExists("-h")) {
		std::cout << "This program allows user to send commands with input data to 'regulacni system xCC' and read the response" << std::endl;
		std::cout << "Available switches:" << std::endl;
		std::cout << "  -s\tserver address to which command will be sent" << std::endl;
		std::cout << "  -p\tserver port" << std::endl;
		std::cout << "  -c\tcommand to be sent, followed by data delimeted by semicolon, e.g. 1000;true" << std::endl;
		std::cout << "  -d\twhen used, whole message will be printed as hex before send and after receive" << std::endl;
		std::cout << "  -h\tprint this message" << std::endl;
		return 0;
	}

	if (!input.cmdOptionExists("-c")) {
		std::cout << "Swith -c is mandatory, you must enter a command. Use -h to see more" << std::endl;
		return 1; // indicate issue
	}

	if (input.cmdOptionExists("-s")) {
		address = input.getCmdOption("-s");
	}

	if (input.cmdOptionExists("-p")) {
		port = std::stoi(input.getCmdOption("-p"));
	}

	if (input.cmdOptionExists("-d")) {
		debug = true;
	}

	// constructor will parse command from input string
	commandString = input.getCmdOption("-c");

	try {
		Message msg(commandString);
		msg.composeFrame();
		if (debug) {
			printBuffer(msg.m_frame);
		}

		// send
		Connect connection(address, port);
		connection.Send(msg.m_frame);

		// parse response
		connection.Receive(msg.m_frame);
		if (debug) {
			printBuffer(msg.m_frame);
		}
		msg.decomposeFrame();
	}
	catch (const std::exception & ex)
	{
		std::cerr << "Exception: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
