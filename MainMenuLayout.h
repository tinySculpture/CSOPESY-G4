#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "ConsoleLayout.h"
#include "Console.h"

class MainMenuLayout : public ConsoleLayout {
public:
    MainMenuLayout();
    void render() override;
    bool processInput(const std::string input) override;
};
