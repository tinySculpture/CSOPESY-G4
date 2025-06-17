#pragma once

#include "Console.h"
#include "ConsoleLayout.h"
class ProcessScreenLayout : public ConsoleLayout {
private:
    std::shared_ptr<Process> currentProcess;
	void displayProcessInfo();

public:
    ProcessScreenLayout(ConsoleSystem* sys);
    void setCurrentProcess(std::shared_ptr<Process> process);
    void render() override;
    bool handleInput(const std::string& input) override;
};
#pragma once
