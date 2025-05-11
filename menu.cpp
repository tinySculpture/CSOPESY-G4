#include "menu.h";
#include <iostream>;

void menuHeader()
{
	std::cout <<	"---------------------------------------------" << std::endl;
	std::cout <<	"  _____ _____ _____ _____ _____ _____ __ __  " << std::endl <<
					" |     |   __|     |  _  |   __|   __|  |  | " << std::endl <<
					" |   --|__   |  |  |   __|   __|__   |_   _| " << std::endl <<
					" |_____|_____|_____|__|  |_____|_____| |_|   " << "\n" << std::endl;
	std::cout <<	"---------------------------------------------" << std::endl;

	std::cout << "\nWelcome to the CSOPESY Emulator!\n" << std::endl;
	std::cout << "Developers:" << std::endl;
	std::cout << "Luistro, Josiah Mari" << std::endl;
	std::cout << "Rimando, Alonzo Andrei" << std::endl;
	std::cout << "Santos, Andrea Li" << std::endl;
	std::cout << "Vasquez, Ryan Clemence" << std::endl;
	std::cout << "\nLast Updated: 11-5-2025\n" << std::endl; //TODO: Change date updated

	std::cout << "---------------------------------------------" << std::endl;
}