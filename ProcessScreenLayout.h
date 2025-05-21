#pragma once

#include "Console.h"
#include "ConsoleLayout.h"
class ProcessScreenLayout : public ConsoleLayout {
private:
    Process* currentProcess;
public:
    ProcessScreenLayout(ConsoleSystem* sys);
    void setCurrentProcess(Process* p);
    void render() override;
    bool handleInput(const std::string& input) override;
};
#pragma once
