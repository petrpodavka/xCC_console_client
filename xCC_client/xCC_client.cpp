#include <iostream>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include "connect.h"
#include "xCC_protocol.h"
#include "xCC_command.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 61000

int main(int argc, char** argv)
{
	std::vector<uint8_t> buffer;
	int result = 0;

	std::string address = DEFAULT_ADDRESS;
	int port = DEFAULT_PORT;
	
	if (argc > 1)
	{
		address = argv[1];
	}

	if (argc > 2) 
	{
		port = atoi(argv[2]);
	}
	
	result = xCC_connect(address, port);

	if (result != 0) {
		std::cout << "Unable to connect to server " << address << ":" << port << std::endl;
		return 0;
	}
		
	std::cout << "0: exit" << std::endl;
	std::cout << "1: mock OUTSIDE_TEMP" << std::endl;
	std::cout << "2: OUTSIDE_TEMP" << std::endl;
	
	int input;
	while (1)
	{
		Message msg = Message(Operation::get_outside_temp);
		
		std::cin >> input;	
		switch (input) {
		case 0:
			goto exit;
		case 1:
			msg.m_command.addData((float)25.48);
			break;
		case 2:
			break;
		default:
			std::cout << "Unknow command" << std::endl;
		}

		try
		{
			msg.composeFrame();

			xCC_send(msg.m_frame);
			xCC_receive(buffer);

			std::cout << "raw data (" << buffer.size() << " bytes): 0x";
			for (unsigned int i = 0; i < buffer.size(); i++)
			{
				std::cout << std::hex << std::internal << std::setfill('0') << std::setw(2) << (int) (unsigned char) buffer[i];
			}
			std::cout << std::endl;

			msg.m_frame = buffer;
			msg.decomposeFrame();

			float temp;
			msg.m_command.readData(&temp);

			std::cout << "Outside temperature is " << std::fixed << std::setprecision(2) << temp << " C" << std::endl;
		}
		catch (const std::exception & ex)
		{
			std::cerr << "Exception: " << ex.what() << std::endl;
			xCC_shutdown_and_cleanup();
			return 1;
		}
	}

exit:
	xCC_shutdown_and_cleanup();

	return 0;
}
