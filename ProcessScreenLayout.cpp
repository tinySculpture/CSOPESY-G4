#include "ProcessScreenLayout.h"
#include "ColorUtils.h"
#include "ConsoleLayout.h"
#include "Console.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using Color = ColorUtils::Color;
namespace CU = ColorUtils;

ProcessScreenLayout::ProcessScreenLayout(ConsoleSystem* sys)
    : ConsoleLayout(sys, "ProcessScreen"), currentProcess(nullptr) {
}

void ProcessScreenLayout::setCurrentProcess(Process* p) {
    currentProcess = p;
}

void ProcessScreenLayout::render() {
    system->clearScreen();

    if (currentProcess) {
        ColorUtils::printColoredText(ColorUtils::Color::LightGreen, currentProcess->getName());
        std::cout << "\n";

        int current = currentProcess->getCurrentInstruction();
        int total = currentProcess->getTotalInstructions();

        CU::printColoredText(Color::Yellow,
            std::to_string(current) + " / " + std::to_string(total) +
            " Instructions\n");

        CU::printColoredText(Color::White, "Created: ");
        std::cout << currentProcess->getTimestamp() << "\n";
    }
    else {
        CU::printColoredText(Color::Red, "No process selected.\n");
    }
}

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