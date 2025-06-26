#include <algorithm>
#include <ctime>
#include <iostream>
#include <sstream>
#include <windows.h>

#include "FCFSScheduler.h"
#include "RRScheduler.h"
#include "Console.h"
#include "MainMenu.h"
#include "ProcessScreen.h"
#include "ColorUtil.h"

ConsoleSystem* ConsoleSystem::sharedInstance = nullptr;

ConsoleSystem::ConsoleSystem() {
    running = true;
    initialized = false;

    layouts["MainMenu"] = std::make_shared<MainMenu>();
    layouts["ProcessScreen"] = std::make_shared<ProcessScreen>();
    currentLayout = layouts["MainMenu"];
    lastLayout = nullptr;
}

void ConsoleSystem::initialize() {
    sharedInstance = new ConsoleSystem();
}

void ConsoleSystem::destroy() {
    delete sharedInstance;
    sharedInstance = nullptr;
}

ConsoleSystem* ConsoleSystem::getInstance() {
    return sharedInstance;
}

void ConsoleSystem::drawConsole() {
    if (currentLayout != lastLayout) {
        currentLayout->render();
        lastLayout = currentLayout;
    }
}

void ConsoleSystem::processInput() {
    std::string input;
    ColorUtil::printPrompt();
    std::getline(std::cin, input);
    currentLayout->processInput(input);
}

void ConsoleSystem::switchLayout(std::string layoutName) {
    if (layouts.find(layoutName) != layouts.end()) {
        system("cls");
        lastLayout = nullptr;
        currentLayout = layouts[layoutName];
    }
    else {
        std::cerr << "Error: Layout '" << layoutName << "' does not exist." << std::endl;
    }
}

void ConsoleSystem::switchLayout(const std::string layoutName, std::shared_ptr<Process> process) {
    auto it = layouts.find(layoutName);
    if (it != layouts.end()) {
        system("cls");
        lastLayout = currentLayout;
        currentLayout = it->second;
        if (layoutName == "ProcessScreen") {
            auto* layout = dynamic_cast<ProcessScreen*>(currentLayout.get());
            if (layout) {
                layout->setCurrentProcess(process);
            }
        }
    }
}

void ConsoleSystem::configure(const std::string& configFile) {
    config = SystemConfig::loadFromFile(configFile);
    config.printSystemConfig();

    if (config.scheduler == "rr") {
        scheduler = std::make_unique<RRScheduler>(config);
    }
    else if (config.scheduler == "fcfs") {
        scheduler = std::make_unique<FCFSScheduler>(config);
    }

    initialized = true;
}

const SystemConfig& ConsoleSystem::getConfig() const {
    return config;
}

Scheduler* ConsoleSystem::getScheduler() {
    return scheduler.get();
}

bool ConsoleSystem::isRunning() {
    return running;
}

bool ConsoleSystem::isInitialized() {
    return initialized;
}

void ConsoleSystem::exit() {
    running = false;
}