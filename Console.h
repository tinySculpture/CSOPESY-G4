#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Process.h"
#include "SystemConfig.h"
#include "Scheduler.h"

class ConsoleLayout;

class ConsoleSystem {
public:
    static ConsoleSystem* getInstance();
    static void initialize();
    static void destroy();

    void drawConsole();
    void processInput();
    void switchLayout(std::string layoutName);
    void switchLayout(std::string layoutName, std::shared_ptr<Process> process);

    void configure(const std::string& configFile);
    const SystemConfig& getConfig() const;
    Scheduler* getScheduler();

    bool isRunning();
    bool isInitialized();

    void exit();

private:
    ConsoleSystem();
    ~ConsoleSystem() = default;
    ConsoleSystem(ConsoleSystem const&) {};
    ConsoleSystem& operator=(ConsoleSystem const&) { return *this; }

    static ConsoleSystem* sharedInstance;

    std::map<std::string, std::shared_ptr<ConsoleLayout>> layouts;
    std::shared_ptr<ConsoleLayout> currentLayout;
    std::shared_ptr<ConsoleLayout> lastLayout;

    bool running = true;
    bool initialized = false;
    SystemConfig config;
    std::unique_ptr<Scheduler> scheduler;
};