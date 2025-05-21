#pragma once

#include "Process.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

class ConsoleLayout;

class ConsoleSystem {
private:
    std::vector<Process> processes;
    std::map<std::string, std::shared_ptr<ConsoleLayout>> layouts;
    std::shared_ptr<ConsoleLayout> currentLayout;
    bool isRunning = false;

public:
    ConsoleSystem();

    void initialize();
    void run();
    void exit();

    void switchLayout(const std::string& layoutName);
    void switchLayout(const std::string& layoutName, Process* process);

    void clearScreen();
    Process* createProcess(const std::string& name);
    Process* findProcess(const std::string& name);
    const std::vector<Process>& getAllProcesses() const;


    std::shared_ptr<ConsoleLayout> getCurrentLayout() const { return currentLayout; }

    bool processInput(const std::string& input);
    std::vector<std::string> tokenizeInput(const std::string& input);

    static std::string getCurrentTimestamp();
    std::string getCurrentLayoutName() const;
};
