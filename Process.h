#pragma once

#include <string>

class Process {

public:
    explicit Process(const std::string& name);

    std::string getName() const;
    int getCurrentInstruction() const;
    int getTotalInstructions() const;
    std::string getTimestamp() const;

private:
    std::string name;
    int currentInstruction;
    int totalInstructions;
    std::string timestamp;
    std::string generateTimestamp() const;
};
