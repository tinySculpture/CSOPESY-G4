#include <iostream>
#include <iomanip>
#include <sstream>

#include "Console.h"
#include "ConsoleLayout.h"
#include "ProcessScreen.h"
#include "ColorUtil.h"
#include "ConsoleUtil.h"

using Color = ColorUtil::Color;
namespace CU = ColorUtil;

ProcessScreen::ProcessScreen()
    : ConsoleLayout("ProcessScreen") {
}

void ProcessScreen::render() {
    displayProcessInfo();
}

void ProcessScreen::setCurrentProcess(std::shared_ptr<Process> process) {
    currentProcess = process;
}

void ProcessScreen::displayProcessInfo() {
    auto& p = currentProcess;
    std::cout
        << "PID          : " << p->getPID() << "\n"
        << "Process Name : " << p->getName() << "\n"
        << "Created      : " << p->getCreationTime() << "\n"
        << "Instructions : " << p->getCurrentInstructionIndex() << "\\" << p->getTotalInstructions() << "\n";
}

void ProcessScreen::displayProcessLogs() {
    auto& p = currentProcess;
    std::cout << "Logs        : ";
    if (p->getLogs().empty()) {
        CU::printColoredText(Color::Red, "[!] No logs available.\n");
    }
    else {
        std::cout << "\n";
        for (const auto& log : p->getLogs()) {
            std::cout << std::left
                << std::setw(24) << log.timestamp
                << "Core: " << std::setw(4) << log.coreID
                << std::setw(10) << log.instruction << "\n";
        }
    }
}

bool ProcessScreen::processInput(const std::string input) {
    std::vector<std::string> tokens = ConsoleUtil::tokenizeInput(input);

    if (tokens.empty()) {
        return false;
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        ConsoleSystem::getInstance()->switchLayout("MainMenu");
    }
    else if (command == "clear") {
        ConsoleSystem::getInstance()->switchLayout("ProcessScreen");
        ConsoleSystem::getInstance()->drawConsole();
    }
    else if (command == "process-smi") {
        std::cout << "\n";
        displayProcessLogs();
    }
    else {
        CU::printColoredText(Color::Red, "Invalid Command. Please try again.\n");
    }

    return false;
}