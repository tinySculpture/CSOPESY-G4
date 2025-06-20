#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <windows.h>

#include "ColorUtils.h"
#include "Console.h"
#include "MainMenuLayout.h"
#include "ProcessScreenLayout.h"

ConsoleSystem* ConsoleSystem::sharedInstance = nullptr;

ConsoleSystem::ConsoleSystem()
{
	running = true;

    // Initialize consoles
    layouts["MainMenu"] = std::make_shared<MainMenuLayout>();
    layouts["ProcessScreen"] = std::make_shared<ProcessScreenLayout>();
    currentLayout = layouts["MainMenu"];
    lastLayout = nullptr;
}



void ConsoleSystem::initialize() {
	sharedInstance = new ConsoleSystem();
}



void ConsoleSystem::drawConsole() {
    // Render layout only if it's different from the last one
    if (currentLayout != lastLayout) {
        currentLayout->render();
        lastLayout = currentLayout;
    }
}



void ConsoleSystem::processInput() {
    // Prompt and read user input
    std::string input;
    ColorUtils::printPrompt();
    std::getline(std::cin, input);
    currentLayout->processInput(input);
}



void ConsoleSystem::exit() {
    running = false;
}



void ConsoleSystem::destroy() {
    delete sharedInstance;
}



void ConsoleSystem::switchLayout(std::string layoutName) {
    if (layouts.contains(layoutName))
    {
		system("cls"); // Clear the console screen
		lastLayout = currentLayout; // Store the last layout
        currentLayout = layouts[layoutName]; // Switch to the new layout
    }
    else
    {
        std::cerr << "Error: Layout '" << layoutName << "' does not exist." << std::endl;
    }
}



void ConsoleSystem::switchLayout(const std::string layoutName, std::shared_ptr<Process> process) {
    auto it = layouts.find(layoutName);
    if (it != layouts.end()) {
        system("cls");
		lastLayout = currentLayout; // Store the last layout
        currentLayout = it->second;
        // If the layout is ProcessScreen, assign the current process
        if (layoutName == "ProcessScreen") {
            auto* layout = dynamic_cast<ProcessScreenLayout*>(currentLayout.get());
            if (layout) {
                layout->setCurrentProcess(process);
            }
        }
    }
}



ConsoleSystem* ConsoleSystem::getInstance() {
	return sharedInstance;
}



bool ConsoleSystem::isRunning() {
    return running;
}