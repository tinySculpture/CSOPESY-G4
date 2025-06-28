#pragma once

#include <string>

class ConsoleSystem;

class ConsoleLayout {

protected:
    std::string name;

public:
    ConsoleLayout(const std::string& layoutName)
        : name(layoutName) {
    }

    virtual ~ConsoleLayout() = default;

    virtual void render() = 0;

    virtual bool processInput(const std::string input) = 0;

    std::string getName() const { return name; }
};