#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "Process.h"
#include "Scheduler.h"
#include "Console.h"
#include "ConsoleLayout.h"

class MainMenu : public ConsoleLayout {
public:
    MainMenu();
    void render() override;
    bool processInput(const std::string input) override;

private:
    static std::atomic<bool> testingScheduler;
    static std::thread testThread;

    void screenStart(const std::string& name);
    void screenResume(const std::string& name);
    void screenList();
    void schedulerStart();
    void schedulerStop();
    void reportUtil();
};