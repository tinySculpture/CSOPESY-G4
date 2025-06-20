#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Process.h"

class ConsoleLayout;

class ConsoleSystem {
private:
    ConsoleSystem();
    ~ConsoleSystem() = default;
    ConsoleSystem(ConsoleSystem const&) {};
    ConsoleSystem& operator=(ConsoleSystem const&) {};
	static ConsoleSystem* sharedInstance;

	std::map<std::string, std::shared_ptr<ConsoleLayout>> layouts;
    std::shared_ptr<ConsoleLayout> currentLayout;
	std::shared_ptr<ConsoleLayout> lastLayout;
    bool running = true;

public:
	static ConsoleSystem* getInstance();
    static void initialize();
    static void destroy();

    void drawConsole();
    void processInput();
    void switchLayout(std::string layoutName);
    void switchLayout(std::string layoutName, std::shared_ptr<Process> process);
    
	bool isRunning();

    void exit();
};
