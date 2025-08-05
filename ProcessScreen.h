#pragma once

#include "Console.h"
#include "ConsoleLayout.h"

/**
 * @class ProcessScreen
 * @brief Represents a console-based UI layout for displaying and interacting with a process.
 *
 * Inherits from ConsoleLayout and provides functionality to render process information,
 * handle user input, and display logs related to a specific process.
 */
class ProcessScreen : public ConsoleLayout {
public:
    ProcessScreen();
    void render() override;
    bool processInput(const std::string input);

    void setCurrentProcess(std::shared_ptr<Process> process);

private:
    std::shared_ptr<Process> currentProcess;

    void displayProcessInfo();
    void displayProcessLogs();
};