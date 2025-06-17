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

std::string MainMenuLayout::truncateLongNames(std::string name) {
    if (name.size() > 9) {
        const size_t maxDisplay = 9;       // total chars we’ll display
        const size_t visible = maxDisplay - 3; // chars after the "..."
        name = "..." + name.substr(name.size() - visible);
    }

    return name;
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
 * - `exit`: Terminates the console system. (done)
 * - `initialize`: Placeholder for future feature. (done)
 * - `scheduler-test`: Placeholder for testing scheduling. (done)
 * - `scheduler-stop`: Placeholder for stopping scheduler. (done)
 * - `report-util`: Placeholder for utility report.
 * - `clear`: Clears the screen and re-renders the layout. (done)
 * - `screen -s <name>`: Creates a new process and opens ProcessScreenLayout. (done)
 * - `screen -r <name>`: Resumes an existing process in ProcessScreenLayout. (done)
 * - `screen -ls`: Lists all created processes with their current and total instructions. (done)
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
		system->getScheduler()->stop(); // Ensure scheduler is stopped before exiting
        system->exit();
    }
    // Placeholder commands
    else if (command == "initialize") {
        if (!system->isSchedulerRunning()) {
            system->getScheduler()->start();
            system->startScheduler();
            CU::printColoredText(Color::Green, "Scheduler started.\n");
        }
        else {
            CU::printColoredText(Color::Yellow, "Scheduler is already running.\n");
        }
    }
    else if (command == "scheduler-test") {
        CU::printColoredText(Color::Green, "Starting scheduler test...\n");
        if (!tickRunning) {
            tickRunning = true;
            tickThread = std::thread([this]() {
                while (tickRunning) {
                    std::string name = "TickProc" + std::to_string(++tickCount);
					auto newProcess = std::make_shared<Process>(name, 100); // Create a new process with 1000 instructions
					system->getScheduler()->addProcess(newProcess);
					std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate a tick every second
                }
			});
        }
    }
    else if (command == "scheduler-stop") {
        CU::printColoredText(Color::Green, "Stopping scheduler test...\n");
        tickRunning = false;
        if (tickThread.joinable()) tickThread.join();
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
            if (!system->findProcessByName(process_name)) {
				auto newProcess = std::make_shared<Process>(process_name, 100); // Create a new process with 1000 instructions
                system->getScheduler()->addProcess(newProcess);
                CU::printColoredText(Color::Green, "Process '" + process_name + "' created successfully.\n");
                system->switchLayout("ProcessScreen", newProcess);
            }
            else {
                CU::printColoredText(Color::Red, "Process '" + process_name + "' already exists.\n");
            }
        }
        // Resume an existing process
        else if (tokens.size() == 3 && tokens[1] == "-r") {
            std::string process_name = tokens[2];
			auto proc = system->findProcessByName(process_name);
            if (proc) {
                system->switchLayout("ProcessScreen", proc);
            }
            else {
                CU::printColoredText(Color::Red, "Process '" + process_name + "' not found.\n");
            }
        }
        //// List all created processes
        else if (tokens.size() == 2 && tokens[1] == "-ls") {
            std::cout << "-----------------------------------------------------------------" << std::endl;

            std::cout << "System CPU ticks: "
                << system->getScheduler()->getCpuTicks()
                << "\n";

            const std::vector<std::shared_ptr<Process>> allProcesses = system->getScheduler()->getAllProcesses();

            if (allProcesses.empty()) {
                CU::printColoredText(Color::Yellow, "No processes found.\n");
            }
            else {
                CU::printColoredText(Color::Aqua, "Running processes:\n");
                if (!(system->getScheduler()->allCoresFree())) {
                    for (const std::shared_ptr<Process> proc : allProcesses) {
                        if (proc->isComplete()) continue; // Skip finished processes

                        // ——— truncate long names ———
                        std::string name = truncateLongNames(proc->getName());

                        std::cout << std::left
                            << std::setw(12) << name
                            << std::setw(27) << proc->getTimestamp();

                        if (proc->getCoreID() != -1) { // If the process is assigned to a core
                            std::cout << std::left << std::setw(6) << "Core: "
                                << std::setw(7) << proc->getCoreID();
                        }
                        else {
                            // If the process is not assigned to a core
                            std::cout << std::left << std::setw(13) << "Ready";
                        }

                        std::cout << std::left
                            << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + " / " + std::to_string(proc->getTotalInstructions()))
                            << "\n";
                    }
                }
                else {
                    CU::printColoredText(Color::Green, "All processes are executed.\n");
                }

                CU::printColoredText(Color::Aqua, "\nFinished processes:\n");
                if (!(system->getScheduler()->noProcessFinished())) {
                    for (const std::shared_ptr<Process> proc : allProcesses) {
                        if (!proc->isComplete()) continue; // Skip running processes

                        // ——— truncate long names ———
                        std::string name = truncateLongNames(proc->getName());

                        std::cout << std::left
                            << std::setw(12) << name
                            << std::setw(27) << proc->getTimestamp()
                            << std::setw(13) << "Finished"
                            << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + " / " + std::to_string(proc->getTotalInstructions()))
                            << "\n";
                    }
                }
                else {
                    CU::printColoredText(Color::Green, "No processes finished yet.\n");
                }
            }

            std::cout << "-----------------------------------------------------------------" << std::endl;
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
