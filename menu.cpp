#include <iostream>
#include <string>
#include <windows.h>

#include "menu.h"
#include "utils.h"

void displayHeader()
{
	std::cout <<	"---------------------------------------------" << std::endl;
	std::cout <<	"  _____ _____ _____ _____ _____ _____ __ __  " << std::endl <<
					" |     |   __|     |  _  |   __|   __|  |  | " << std::endl <<
					" |   --|__   |  |  |   __|   __|__   |_   _| " << std::endl <<
					" |_____|_____|_____|__|  |_____|_____| |_|   " << "\n" << std::endl;
	std::cout <<	"---------------------------------------------" << std::endl;

	printColoredText(2, "\nWelcome to the CSOPESY Emulator!\n");

	std::cout << "Developers:" << std::endl;
	std::cout << "Luistro, Josiah Mari" << std::endl;
	std::cout << "Rimando, Alonzo Andrei" << std::endl;
	std::cout << "Santos, Andrea Li" << std::endl;
	std::cout << "Vasquez, Ryan Clemence" << std::endl;
	std::cout << "\nLast Updated: ";

	printColoredText(2, "11 - 5 - 2025\n");
	std::cout << "---------------------------------------------" << std::endl;
}

void handleInput(std::string input)
{
	// Initialize command
	if (input.rfind("initialize", 0) == 0)
	{
		std::cout << "Initialize command found." << std::endl;
	}
	// Screen command
	else if (input.rfind("screen", 0) == 0)
	{
		std::cout << "Screen command found." << std::endl;
	}
	// Scheduler-test command
	else if (input.rfind("scheduler-test", 0) == 0)
	{
		std::cout << "Scheduler-test command found." << std::endl;
	}
	// Scheduler-stop command
	else if (input.rfind("scheduler-stop", 0) == 0)
	{
		std::cout << "Scheduler-stop command found." << std::endl;
	}
	// Report-util command
	else if (input.rfind("report-util", 0) == 0)
	{
		std::cout << "report-util command found." << std::endl;
	}
	// Clear command
	else if (input.rfind("clear", 0) == 0)
	{
		//std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the screen
		system("cls"); // Windows command to clear the screen
		displayHeader(); // Re-display the header
	}
	// Exit command
	else if (input.rfind("exit", 0) == 0)
	{
		isRunning = false;
	}
	// default
	else {
		std::cout << "Invalid command. Please try again." << std::endl;
	}
}

void initializeMenu()
{
	isRunning = true; // Start the menu loop
	std::string input;

	displayHeader();

	while (isRunning)
	{
		printColoredText(6, "Please enter a command: ");
		getline(std::cin, input);
		handleInput(input);
	}
}