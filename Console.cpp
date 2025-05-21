#include "ColorUtils.h"
#include "Console.h"
#include "MainMenuLayout.h"
#include "ProcessScreenLayout.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <windows.h>

ConsoleSystem::ConsoleSystem() {
    layouts["MainMenu"] = std::make_shared<MainMenuLayout>(this);
    layouts["ProcessScreen"] = std::make_shared<ProcessScreenLayout>(this);
    currentLayout = layouts["MainMenu"];
}

void ConsoleSystem::initialize() {
    isRunning = true;
}

void ConsoleSystem::run() {
    std::string lastLayout = "";

    while (isRunning) {
        std::string currentLayoutName = getCurrentLayoutName();

        if (currentLayoutName != lastLayout) {
            currentLayout->render();
            lastLayout = currentLayoutName;
        }

        std::string input;
        ColorUtils::printPrompt();
        std::getline(std::cin, input);
        processInput(input);
    }
}

void ConsoleSystem::exit() {
    isRunning = false;
}

void ConsoleSystem::switchLayout(const std::string& layoutName) {
    switchLayout(layoutName, nullptr);
}

void ConsoleSystem::switchLayout(const std::string& layoutName, Process* process) {
    auto it = layouts.find(layoutName);
    if (it != layouts.end()) {
        currentLayout = it->second;
        if (layoutName == "ProcessScreen") {
            auto* layout = dynamic_cast<ProcessScreenLayout*>(currentLayout.get());
            if (layout) {
                layout->setCurrentProcess(process);
            }
        }
    }
}

Process* ConsoleSystem::createProcess(const std::string& name) {
    auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
        return p.getName() == name;
        });

    if (it != processes.end()) {
        return nullptr;
    }

    processes.emplace_back(name);
    return &processes.back();
}

Process* ConsoleSystem::findProcess(const std::string& name) {
    auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
        return p.getName() == name;
        });

    return (it != processes.end()) ? &(*it) : nullptr;
}

const std::vector<Process>& ConsoleSystem::getAllProcesses() const {
    return processes;
}


bool ConsoleSystem::processInput(const std::string& input) {
    return currentLayout->handleInput(input);
}

std::vector<std::string> ConsoleSystem::tokenizeInput(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

void ConsoleSystem::clearScreen() {
    system("cls");
}

// ==========

std::string ConsoleSystem::getCurrentTimestamp() {
    std::time_t t = std::time(nullptr);
    std::tm now;
    localtime_s(&now, &t);

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &now);

    return buffer;
}

std::string ConsoleSystem::getCurrentLayoutName() const {
    for (const auto& pair : layouts) {
        if (pair.second == currentLayout) {
            return pair.first;
        }
    }
    return "";
}
