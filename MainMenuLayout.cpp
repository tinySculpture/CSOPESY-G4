#include "MainMenuLayout.h"
#include "ColorUtils.h"
#include "ConsoleUtil.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "GlobalScheduler.h"

using Color = ColorUtils::Color;
namespace CU = ColorUtils;

int dummyProcessCount = 0; // Counter for dummy processes
bool testingScheduler = false; // Flag to indicate if the scheduler is being tested
std::thread testThread; // Thread for producing dummy processes



MainMenuLayout::MainMenuLayout()
    : ConsoleLayout("MainMenu") {
}



void MainMenuLayout::render() {
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



bool MainMenuLayout::processInput(const std::string input) {
    std::vector<std::string> tokens = ConsoleUtil::tokenizeInput(input);

    if (tokens.empty()) {
        std::cout << "Error! Empty input.\n";
        return false;
    }

    const std::string& command = tokens[0];

    // Command: Exit program
    if (command == "exit") {
		GlobalScheduler::getInstance()->stop(); // Stop the global scheduler if running
        ConsoleSystem::getInstance()->exit();
    }
    // Placeholder commands
    else if (command == "initialize") {
        if (!GlobalScheduler::getInstance()->isRunning()) {
            GlobalScheduler::getInstance()->start();
            CU::printColoredText(Color::Green, "Scheduler started.\n");
        }
        else {
            CU::printColoredText(Color::Yellow, "Scheduler is already running.\n");
        }
    }
    else if (command == "scheduler-start" && GlobalScheduler::getInstance()->isRunning()) {
        CU::printColoredText(Color::Green, "Starting scheduler test...\n");
        if (!testingScheduler) {
            testingScheduler = true;
            testThread = std::thread([this]() {
                while (testingScheduler) {
                    std::string name = "TickProc" + std::to_string(++dummyProcessCount);

                    if (!ConsoleUtil::findProcessByName(name)) {
                        auto newProcess = std::make_shared<Process>(name, 100); // Create a new process with 1000 instructions
                        GlobalScheduler::getInstance()->addProcess(newProcess);
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate a tick every second
                }
			});
        }
    }
    else if (command == "scheduler-stop" && GlobalScheduler::getInstance()->isRunning()) {
        CU::printColoredText(Color::Green, "Stopping scheduler test...\n");
        testingScheduler = false;
        if (testThread.joinable()) testThread.join();
    }
    else if (command == "report-util" && GlobalScheduler::getInstance()->isRunning()) {
        std::ofstream reportFile("csopesy-log.txt");
        if (!reportFile.is_open()) {
            CU::printColoredText(Color::Red, "Failed to open report file.\n");
            return 1;
		}

        reportFile << "CSOPESY Emulator Report\n\n";
        reportFile << "-----------------------------------------------------------------" << std::endl;

        const std::vector<std::shared_ptr<Process>> allProcesses = GlobalScheduler::getInstance()->getAllProcesses();

        if (allProcesses.empty()) {
			reportFile << "No processes found." << std::endl;
        }
        else {
			reportFile << "Running processes:" << std::endl;
            if (!(GlobalScheduler::getInstance()->allCoresFree())) {
                for (const std::shared_ptr<Process> proc : allProcesses) {
                    if (proc->isComplete()) continue; // Skip finished processes

                    // ——— truncate long names ———
                    std::string name = ConsoleUtil::truncateLongNames(proc->getName());

                    reportFile << std::left
                        << std::setw(12) << name
                        << std::setw(27) << proc->getTimestamp();

                    if (proc->getCoreID() != -1) { // If the process is assigned to a core
                        reportFile << std::left << std::setw(6) << "Core: "
                            << std::setw(7) << proc->getCoreID();
                    }
                    else {
                        // If the process is not assigned to a core
                        reportFile << std::left << std::setw(13) << "Ready";
                    }

                    reportFile << std::left
                        << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + " / " + std::to_string(proc->getTotalInstructions()))
                        << "\n";
                }
            }
            else {
				reportFile << "All processes are executed." << std::endl;
            }

			reportFile << "\nFinished processes:" << std::endl;
            if (!(GlobalScheduler::getInstance()->noProcessFinished())) {
                for (const std::shared_ptr<Process> proc : allProcesses) {
                    if (!proc->isComplete()) continue; // Skip running processes

                    // ——— truncate long names ———
                    std::string name = ConsoleUtil::truncateLongNames(proc->getName());

                    reportFile << std::left
                        << std::setw(12) << name
                        << std::setw(27) << proc->getTimestamp()
                        << std::setw(13) << "Finished"
                        << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + " / " + std::to_string(proc->getTotalInstructions()))
                        << "\n";
                }
            }
            else {
				reportFile << "No processes finished yet." << std::endl;
            }
        }

        reportFile << "-----------------------------------------------------------------" << std::endl;
        reportFile.close();
		CU::printColoredText(Color::Green, "Report generated successfully.\n");
    }
    // Command: Clear screen
    else if (command == "clear") {
		ConsoleSystem::getInstance()->switchLayout("MainMenu"); // Switch to MainMenu layout to clear the screen
        ConsoleSystem::getInstance()->drawConsole();
    }
    // Command group: Process management and screen switch
    else if (command == "screen" && GlobalScheduler::getInstance()->isRunning()) {
        // Create and switch to a new process screen
        if (tokens.size() == 3 && tokens[1] == "-s") {
			std::string process_name = tokens[2];
            if (!ConsoleUtil::findProcessByName(process_name)) {
				auto newProcess = std::make_shared<Process>(process_name, 100); // Create a new process with 1000 instructions
                GlobalScheduler::getInstance()->addProcess(newProcess);
                CU::printColoredText(Color::Green, "Process '" + process_name + "' created successfully.\n");
                ConsoleSystem::getInstance()->switchLayout("ProcessScreen", newProcess);
            }
            else {
                CU::printColoredText(Color::Red, "Process '" + process_name + "' already exists.\n");
            }
        }
        // Resume an existing process
        else if (tokens.size() == 3 && tokens[1] == "-r") {
            std::string process_name = tokens[2];
			auto proc = ConsoleUtil::findProcessByName(process_name);
            if (proc) {
                ConsoleSystem::getInstance()->switchLayout("ProcessScreen", proc);
            }
            else {
                CU::printColoredText(Color::Red, "Process '" + process_name + "' not found.\n");
            }
        }
        //// List all created processes
        else if (tokens.size() == 2 && tokens[1] == "-ls") {
            std::cout << "-----------------------------------------------------------------" << std::endl;

            const std::vector<std::shared_ptr<Process>> allProcesses = GlobalScheduler::getInstance()->getAllProcesses();

            if (allProcesses.empty()) {
                CU::printColoredText(Color::Yellow, "No processes found.\n");
            }
            else {
                CU::printColoredText(Color::Aqua, "Running processes:\n");
                if (!(GlobalScheduler::getInstance()->allCoresFree())) {
                    for (const std::shared_ptr<Process> proc : allProcesses) {
                        if (proc->isComplete()) continue; // Skip finished processes

                        // ——— truncate long names ———
                        std::string name = ConsoleUtil::truncateLongNames(proc->getName());

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
                if (!(GlobalScheduler::getInstance()->noProcessFinished())) {
                    for (const std::shared_ptr<Process> proc : allProcesses) {
                        if (!proc->isComplete()) continue; // Skip running processes

                        // ——— truncate long names ———
                        std::string name = ConsoleUtil::truncateLongNames(proc->getName());

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