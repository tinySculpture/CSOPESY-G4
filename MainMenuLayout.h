#pragma once

#include "ConsoleLayout.h"
#include "Console.h"

class MainMenuLayout : public ConsoleLayout {
public:
    explicit MainMenuLayout(ConsoleSystem* console);
    void render() override;
    bool handleInput(const std::string& input) override;

private:
    ConsoleSystem* console;
    
    /** Background thread that creates a new process every tick */
    std::thread          tickThread;

    /** Flag to control the tick thread */
    std::atomic<bool>    tickRunning{ false };

    /** Mutex for serializing console output in the tick thread */
    static std::mutex    coutMutex;

	int tickCount = 0;

    // Flag to check if console is initialized
    bool isInitialized = false;

    std::string truncateLongNames(std::string name);
};
