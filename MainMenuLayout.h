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
};
