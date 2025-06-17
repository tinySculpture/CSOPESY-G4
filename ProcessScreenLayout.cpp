#include "ProcessScreenLayout.h"
#include "ColorUtils.h"
#include "ConsoleLayout.h"
#include "Console.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using Color = ColorUtils::Color;
namespace CU = ColorUtils;

/**
 * @class ProcessScreenLayout
 * @brief Represents the layout for displaying details of a specific process
 *        within the console-based UI system.
 *
 * This layout shows the name of the selected process, its current instruction
 * number, total instructions, and creation timestamp. It supports minimal
 * command input such as returning to the main menu.
 */

 /**
  * @brief Constructs a new ProcessScreenLayout and registers it with the ConsoleSystem.
  *
  * @param sys Pointer to the ConsoleSystem managing this layout.
  */
ProcessScreenLayout::ProcessScreenLayout(ConsoleSystem* sys)
    : ConsoleLayout(sys, "ProcessScreen"), currentProcess(nullptr) {
}

/**
 * @brief Sets the currently active process to be displayed in the layout.
 *
 * @param p Pointer to the Process object to display.
 */
void ProcessScreenLayout::setCurrentProcess(std::shared_ptr<Process> process) {
    currentProcess = process;
}

/**
 * @brief Renders the process details on the console.
 *
 * Displays the process name in green, its instruction progress in yellow,
 * and the creation timestamp in white. If no process is selected,
 * displays an error message in red.
 */
void ProcessScreenLayout::render() {
    system->clearScreen(); // Clear the console screen
	displayProcessInfo(); // Display the current process information
    
}

void ProcessScreenLayout::displayProcessInfo() {
    if (currentProcess) {
        std::cout
            << "Process name: " << currentProcess->getName() << "\n"
            << "ID: " << currentProcess->getPID() << "\n"
			<< "Created: " << currentProcess->getTimestamp() << "\n";

        // Display logs
        std::cout << "Logs:\n";
        if (currentProcess->getLogs().empty()) {
            CU::printColoredText(Color::Red, "No logs available.\n");
		}
        else {
            for (const auto& log : currentProcess->getLogs()) {
                std::cout << std::left
                    << std::setw(24) << log.timestamp
                    << "Core: " << std::setw(4) << log.coreID
                    << std::setw(10) << log.instruction << "\n";
            }
        }

		std::cout << "\n";

		// Show current instruction number
		std::cout 
			<< "Current Instruction: "
            << currentProcess->getTotalInstructions() - currentProcess->getRemainingInstruction() << "\n"
			<< "Lines of code: "
			<< currentProcess->getTotalInstructions() << "\n";

    }
    else {
        // Inform user if no process is selected
        CU::printColoredText(Color::Red, "No process selected.\n");
    }
}

/**
 * @brief Handles user input while this layout is active.
 *
 * Recognized commands:
 * - `exit`: Returns to the MainMenu layout.
 * - Any other input results in an error message.
 *
 * @param input The raw user input string.
 * @return true if input was handled successfully, false otherwise.
 */
bool ProcessScreenLayout::handleInput(const std::string& input) {
    std::vector<std::string> tokens = system->tokenizeInput(input);

    if (tokens.empty()) {
        return false;
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        system->switchLayout("MainMenu");
    }
    else if (command == "process-smi") {
        std::cout << "\n";

		displayProcessInfo(); // Display the current process information
    }
    else {
        CU::printColoredText(Color::Red, "Invalid Command. Please try again.\n");
    }

    return false;
}
