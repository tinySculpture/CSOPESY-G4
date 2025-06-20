#include "ColorUtils.h"
#include "Console.h"
#include "MainMenuLayout.h"
#include "ProcessScreenLayout.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <windows.h>

/**
 * @class ConsoleSystem
 * @brief Manages the console-based user interface system, allowing layout switching,
 *        user input handling, and process management within a terminal application.
 */

 /**
  * @brief Constructs a new ConsoleSystem object.
  *        Initializes all available layouts and sets the default layout to "MainMenu".
  */
ConsoleSystem::ConsoleSystem() {
    layouts["MainMenu"] = std::make_shared<MainMenuLayout>(this);
    layouts["ProcessScreen"] = std::make_shared<ProcessScreenLayout>(this);
    currentLayout = layouts["MainMenu"];
}

/**
 * @brief Initializes the console system by marking it as running.
 */
void ConsoleSystem::initialize() {
    scheduler = std::make_unique<FCFSScheduler>(4);
	schedulerRunning = false;
    isRunning = true;
}

/**
 * @brief Main loop of the application.
 *        Continuously listens for user input and updates the console display as needed.
 *        Renders the layout only when it has changed to optimize screen updates.
 */
void ConsoleSystem::run() {
    std::string lastLayout = "";

    while (isRunning) {
        std::string currentLayoutName = getCurrentLayoutName();

        // Render layout only if it's different from the last one
        if (currentLayoutName != lastLayout) {
            currentLayout->render();
            lastLayout = currentLayoutName;
        }

        // Prompt and read user input
        std::string input;
        ColorUtils::printPrompt();
        std::getline(std::cin, input);
        processInput(input);
    }
}

/**
 * @brief Signals the application to exit by stopping the run loop.
 */
void ConsoleSystem::exit() {
    isRunning = false;
}

/**
 * @brief Switches to a new layout using the given layout name.
 *        Does not pass a Process*.
 *
 * @param layoutName The name of the layout to switch to.
 */
void ConsoleSystem::switchLayout(const std::string& layoutName) {
    switchLayout(layoutName, nullptr);
}

/**
 * @brief Switches to a new layout and optionally sets a Process
 *        if the target layout supports it (e.g., ProcessScreenLayout).
 *
 * @param layoutName The name of the layout to switch to.
 * @param process Pointer to the Process object to associate with the layout.
 */
void ConsoleSystem::switchLayout(const std::string& layoutName, std::shared_ptr<Process> process) {
    auto it = layouts.find(layoutName);
    if (it != layouts.end()) {
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

///**
// * @brief Creates a new Process object if a process with the given name does not already exist.
// *
// * @param name The name of the new process.
// * @return Process* Pointer to the created Process, or nullptr if the name already exists.
// */
//Process* ConsoleSystem::createProcess(const std::string& name) {
//    auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
//        return p.getName() == name;
//        });
//
//    if (it != processes.end()) {
//        return nullptr; // Duplicate name, do not create
//    }
//
//    processes.emplace_back(name, 10);
//    return &processes.back();
//}
//


std::shared_ptr<Process> ConsoleSystem::findProcessByName(const std::string& name) {
    // Assume getScheduler()->getAllProcesses() returns a vector<shared_ptr<Process>>
    const auto all = getScheduler()->getAllProcesses();
    for (const auto& p : all) {
        if (p && p->getName() == name) {
            return p;
        }
    }
    return nullptr;
}



//
///**
// * @brief Retrieves all processes managed by the system.
// *
// * @return const std::vector<Process>& A constant reference to the list of processes.
// */
//const std::vector<Process>& ConsoleSystem::getAllProcesses() const {
//    return processes;
//}

/**
 * @brief Passes the user's input to the currently active layout for processing.
 *
 * @param input The raw string input from the user.
 * @return true If the input was successfully handled by the layout.
 * @return false If the layout did not handle the input.
 */
bool ConsoleSystem::processInput(const std::string& input) {
    return currentLayout->handleInput(input);
}

/**
 * @brief Splits a given input string into individual tokens using whitespace as a delimiter.
 *
 * @param input The input string to tokenize.
 * @return std::vector<std::string> A list of tokens parsed from the input.
 */
std::vector<std::string> ConsoleSystem::tokenizeInput(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

/**
 * @brief Clears the console screen using the Windows-specific "cls" command.
 */
void ConsoleSystem::clearScreen() {
    system("cls");
}

// ===== Utility Functions =====

/**
 * @brief Retrieves the current system time formatted as a string.
 *
 * @return std::string The formatted timestamp: "MM/DD/YYYY, HH:MM:SS AM/PM".
 */
std::string ConsoleSystem::getCurrentTimestamp() {
    std::time_t t = std::time(nullptr);
    std::tm now;
    localtime_s(&now, &t);

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &now);

    return buffer;
}

/**
 * @brief Finds the name of the currently active layout.
 *
 * @return std::string The name of the current layout, or an empty string if not found.
 */
std::string ConsoleSystem::getCurrentLayoutName() const {
    for (const auto& pair : layouts) {
        if (pair.second == currentLayout) {
            return pair.first;
        }
    }
    return "";
}
