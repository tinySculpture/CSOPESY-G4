#include "ProcessScreenLayout.h"
#include "ColorUtils.h"
#include "ConsoleLayout.h"
#include "Console.h"
#include "ConsoleUtil.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using Color = ColorUtils::Color;
namespace CU = ColorUtils;

ProcessScreenLayout::ProcessScreenLayout()
    : ConsoleLayout("ProcessScreen") {
}



void ProcessScreenLayout::setCurrentProcess(std::shared_ptr<Process> process) {
    currentProcess = process;
}



void ProcessScreenLayout::render() {
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



bool ProcessScreenLayout::processInput(const std::string input) {
    std::vector<std::string> tokens = ConsoleUtil::tokenizeInput(input);

    if (tokens.empty()) {
        return false;
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        ConsoleSystem::getInstance()->switchLayout("MainMenu");
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
