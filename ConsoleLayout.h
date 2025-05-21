#pragma once

#include <string>

class ConsoleSystem;

class ConsoleLayout {

protected:
    ConsoleSystem* system;
    std::string name;

public:
    ConsoleLayout(ConsoleSystem* sys, const std::string& layoutName)
        : system(sys), name(layoutName) {
    }

    virtual ~ConsoleLayout() = default;
    virtual void render() = 0;
    virtual bool handleInput(const std::string& input) = 0;

    std::string getName() const { return name; }
};