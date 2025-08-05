#include <algorithm>
#include <ctime>
#include <iostream>
#include <sstream>
#include <windows.h>

#include "Scheduler.h"
#include "FCFSScheduler.h"
#include "RRScheduler.h"
#include "GlobalScheduler.h"
#include "Console.h"
#include "MainMenu.h"
#include "ProcessScreen.h"
#include "ColorUtil.h"

// Static instance pointer for singleton pattern
ConsoleSystem* ConsoleSystem::sharedInstance = nullptr;

// Constructor initializes state and layouts
ConsoleSystem::ConsoleSystem() {
    running = true; // System is running by default
    initialized = false; // Not initialized yet

    // Register available layouts
    layouts["MainMenu"] = std::make_shared<MainMenu>();
    layouts["ProcessScreen"] = std::make_shared<ProcessScreen>();
    currentLayout = layouts["MainMenu"]; // Start with main menu
    lastLayout = nullptr;
}

// Initializes the singleton instance
void ConsoleSystem::initialize() {
    sharedInstance = new ConsoleSystem();
}

// Destroys the singleton instance
void ConsoleSystem::destroy() {
    delete sharedInstance;
    sharedInstance = nullptr;
}

// Returns the singleton instance
ConsoleSystem* ConsoleSystem::getInstance() {
    return sharedInstance;
}

// Draws the console if the layout has changed
void ConsoleSystem::drawConsole() {
    if (currentLayout != lastLayout) {
        currentLayout->render(); // Render current layout
        lastLayout = currentLayout;
    }
}

// Processes user input and delegates to current layout
void ConsoleSystem::processInput() {
    std::string input;
    ColorUtil::printPrompt(); // Show prompt
    std::getline(std::cin, input); // Read input
    currentLayout->processInput(input); // Handle input
}

// Switches to a different layout by name
void ConsoleSystem::switchLayout(std::string layoutName) {
    if (layouts.find(layoutName) != layouts.end()) {
        system("cls"); // Clear screen
        lastLayout = nullptr;
        currentLayout = layouts[layoutName];
    }
    else {
        std::cerr << "Error: Layout '" << layoutName << "' does not exist." << std::endl;
    }
}

// Switches to a layout and sets the current process (for ProcessScreen)
void ConsoleSystem::switchLayout(const std::string layoutName, std::shared_ptr<Process> process) {
    auto it = layouts.find(layoutName);
    if (it != layouts.end()) {
        system("cls"); // Clear screen
        lastLayout = currentLayout;
        currentLayout = it->second;
        if (layoutName == "ProcessScreen") {
            // Set the process for the ProcessScreen layout
            auto* layout = dynamic_cast<ProcessScreen*>(currentLayout.get());
            if (layout) {
                layout->setCurrentProcess(process);
            }
        }
    }
}

// Loads configuration and initializes subsystems
void ConsoleSystem::configure(const std::string& configFile) {
    config = SystemConfig::loadFromFile(configFile); // Load config
    config.printSystemConfig(); // Print config

    MemoryManager::initialize(config); // Initialize memory manager
    GlobalScheduler::initialize(config); // Initialize scheduler

    initialized = true;
}

// Returns the current system configuration
const SystemConfig& ConsoleSystem::getConfig() const {
    return config;
}

// Returns whether the system is running
bool ConsoleSystem::isRunning() {
    return running;
}

// Returns whether the system is initialized
bool ConsoleSystem::isInitialized() {
    return initialized;
}

// Exits the system
void ConsoleSystem::exit() {
    running = false;
}