#include "MainMenuLayout.h"
#include "ColorUtils.h"
#include <iostream>
#include <iomanip>

using Color = ColorUtils::Color;
namespace CU = ColorUtils;

MainMenuLayout::MainMenuLayout(ConsoleSystem* consolePtr) : ConsoleLayout(consolePtr, "MainMenu") {}

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

bool MainMenuLayout::handleInput(const std::string& input) {
    std::vector<std::string> tokens = system->tokenizeInput(input);

    if (tokens.empty()) {
        std::cout << "Error! Empty input.\n";
        return false;
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        system->exit();
    }
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
    else if (command == "clear") {
        system->clearScreen();
        system->getCurrentLayout()->render();
    }
    else if (command == "screen") {
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
                        << std::setw(15) << (std::to_string(proc.getCurrentInstruction()) + "/" + std::to_string(proc.getTotalInstructions()))
                        << proc.getTimestamp() << "\n";
                }
            }
        }
        else {
            CU::printColoredText(Color::Red, "Proper Usage: screen -s|-r <name> or screen -ls.\n");
        }
    }
    else {
        CU::printColoredText(Color::Red, "Invalid Command. Please Try Again.\n");
    }

    return false;
}