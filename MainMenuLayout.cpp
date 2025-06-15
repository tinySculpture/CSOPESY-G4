#include "MainMenuLayout.h"
#include "ColorUtils.h"
#include <iostream>
#include <iomanip>

using Color = ColorUtils::Color;
namespace CU = ColorUtils;

/**
 * @class MainMenuLayout
 * @brief Represents the main entry screen for the console-based emulator.
 *
 * Displays application title, authorship credits, and a set of recognized commands
 * for interacting with or navigating the system. Commands include creating or
 * loading processes, switching screens, clearing the screen, and exiting the program.
 */

 /**
  * @brief Constructs the MainMenuLayout and registers it with the ConsoleSystem.
  *
  * @param consolePtr Pointer to the ConsoleSystem managing the layout.
  */
MainMenuLayout::MainMenuLayout(ConsoleSystem* consolePtr)
    : ConsoleLayout(consolePtr, "MainMenu") {
}

/**
 * @brief Renders the main menu screen, displaying the emulator title,
 *        developer credits, and last update date in colored and formatted output.
 */
void MainMenuLayout::render() {
    system->clearScreen();

    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "  _____ _____ _____ _____ _____ _____ __ __  " << std::endl <<
                 " |     |   __|     |  _  |   __|   __|  |  | " << std::endl <<
                 " |   --|__   |  |  |   __|   __|__   |_   _| " << std::endl <<
                 " |_____|_____|_____|__|  |_____|_____| |_|   " << "\n" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;

    CU::printColoredText(Color::Green, "\nWelcome to the CSOPESY Emulator!\n");

    std::cout << "Developers:" << std::endl;
    std::cout << "Luistro, Josiah Mari" << std::endl;
    std::cout << "Rimando, Alonzo Andrei" << std::endl;
    std::cout << "Santos, Andrea Li" << std::endl;
    std::cout << "Vasquez, Ryan Clemence" << std::endl;
    std::cout << "\nLast Updated: ";

    CU::printColoredText(Color::Green, "21 - 5 - 2025\n");
    std::cout << "---------------------------------------------" << std::endl;
}

/**
 * @brief Handles user input on the main menu layout.
 *
 * Recognized Commands:
 * - `exit`: Terminates the console system.
 * - `initialize`: Placeholder for future feature.
 * - `scheduler-test`: Placeholder for testing scheduling.
 * - `scheduler-stop`: Placeholder for stopping scheduler.
 * - `report-util`: Placeholder for utility report.
 * - `clear`: Clears the screen and re-renders the layout.
 * - `screen -s <name>`: Creates a new process and opens ProcessScreenLayout.
 * - `screen -r <name>`: Resumes an existing process in ProcessScreenLayout.
 * - `screen -ls`: Lists all created processes with their current and total instructions.
 *
 * @param input The raw string entered by the user.
 * @return true if command executed successfully, false otherwise.
 */
bool MainMenuLayout::handleInput(const std::string& input) {
    std::vector<std::string> tokens = system->tokenizeInput(input);

    if (tokens.empty()) {
        std::cout << "Error! Empty input.\n";
        return false;
    }

    const std::string& command = tokens[0];

    // Command: Exit program
    if (command == "exit") {
        system->exit();
    }
    // Placeholder commands
    else if (command == "initialize") {
        CU::printColoredText(Color::Green, "Initialize command recognized. Doing something.\n");
    }
    else if (command == "scheduler-test") {
        CU::printColoredText(Color::Green, "Scheduler-test command recognized. Doing something.\n");
    }
    else if (command == "scheduler-stop") {
        CU::printColoredText(Color::Green, "Scheduler-stop command recognized. Doing something.\n");
    }
    else if (command == "report-util") {
        CU::printColoredText(Color::Green, "Report-util command recognized. Doing something.\n");
    }
    // Command: Clear screen
    else if (command == "clear") {
        system->clearScreen();
        system->getCurrentLayout()->render();
    }
    // Command group: Process management and screen switch
    else if (command == "screen") {
        // Create and switch to a new process screen
        if (tokens.size() == 3 && tokens[1] == "-s") {
            std::string process_name = tokens[2];
            Process* newProcess = system->createProcess(process_name);
            if (newProcess) {
                system->switchLayout("ProcessScreen", newProcess);
            }
            else {
                CU::printColoredText(Color::Red, "Process '" + process_name + "' already exists.\n");
            }
        }
        // Resume an existing process
        else if (tokens.size() == 3 && tokens[1] == "-r") {
            std::string process_name = tokens[2];
            Process* proc = system->findProcess(process_name);
            if (proc) {
                system->switchLayout("ProcessScreen", proc);
            }
            else {
                CU::printColoredText(Color::Red, "Process '" + process_name + "' not found.\n");
            }
        }
        // List all created processes
        else if (tokens.size() == 2 && tokens[1] == "-ls") {
            const std::vector<Process>& allProcesses = system->getAllProcesses();

            if (allProcesses.empty()) {
                CU::printColoredText(Color::Yellow, "No processes found.\n");
            }
            else {
                CU::printColoredText(Color::Aqua, "\nScreen/Process List:\n");
                std::cout << std::left << std::setw(20) << "Name"
                    << std::setw(15) << "Current/Total"
                    << "Timestamp" << "\n";

                for (const Process& proc : allProcesses) {
                    std::cout << std::left << std::setw(20) << proc.getName()
                        << std::setw(15) << (std::to_string(proc.getTotalInstructions()-proc.getRemainingInstruction()) + "/" + std::to_string(proc.getTotalInstructions()))
                        << proc.getTimestamp() << "\n";
                }
            }
        }
        // Invalid screen usage
        else {
            CU::printColoredText(Color::Red, "Proper Usage: screen -s|-r <name> or screen -ls.\n");
        }
    }
    // Invalid command
    else {
        CU::printColoredText(Color::Red, "Invalid Command. Please Try Again.\n");
    }

    return false;
}
