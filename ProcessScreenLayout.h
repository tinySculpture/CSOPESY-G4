#pragma once

#include "Console.h"
#include "ConsoleLayout.h"
class ProcessScreenLayout : public ConsoleLayout {
private:
    std::shared_ptr<Process> currentProcess;
	
	// Utility function for displaying process information
    void displayProcessInfo();

public:
    ProcessScreenLayout();
    void render() override;
    bool processInput(const std::string input);

	// Set the current process before rendering
    void setCurrentProcess(std::shared_ptr<Process> process);
};
