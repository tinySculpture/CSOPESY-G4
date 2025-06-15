#pragma once

#include <string>

class Process {

public:
    explicit Process(const std::string& name, int totalInstructions);
    Process() : Process("", 0) {};

    std::string getName() const;
	int getCurrentInstruction() const;
    int getTotalInstructions() const;
    std::string getTimestamp() const;
	void executeInstruction();
	bool isComplete();

private:
    std::string name;
	int currentInstruction;
    int totalInstructions;
    std::string timestamp;
    std::string generateTimestamp() const;
};
