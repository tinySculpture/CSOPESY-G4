#pragma once

#include <string>

class Process {

public:
    Process(const std::string& name, int currentInstruction = 1, int totalInstructions = 10);

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