#pragma once

#include <string>

class Process {

public:
    Process(const std::string& name, int numInstructions);

    std::string getName() const;
	int getRemainingInstruction() const;
    int getTotalInstructions() const;
    std::string getTimestamp() const;
	void executeInstruction();
	bool isComplete();

private:
    std::string name;
	int remainingInstruction;
    int totalInstructions;
    std::string timestamp;
    std::string generateTimestamp() const;
};
