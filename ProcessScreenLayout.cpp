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
void ProcessScreenLayout::setCurrentProcess(Process* p) {
    currentProcess = p;
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

    if (currentProcess) {
        // Display process name in green
        CU::printColoredText(Color::LightGreen, currentProcess->getName());
        std::cout << "\n";

        // Show progress of instructions (e.g., 3 / 10 Instructions)
        int current = currentProcess->getTotalInstructions() - currentProcess->getRemainingInstruction();
        int total = currentProcess->getTotalInstructions();
        CU::printColoredText(Color::Yellow,
            std::to_string(current) + " / " + std::to_string(total) + " Instructions\n");

        // Display timestamp
        CU::printColoredText(Color::White, "Created: ");
        std::cout << currentProcess->getTimestamp() << "\n";
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
    else {
        CU::printColoredText(Color::Red, "Invalid Command. Please try again.\n");
    }

    return false;
}
