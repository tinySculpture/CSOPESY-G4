#pragma once

#include "Console.h"
#include "ConsoleLayout.h"

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