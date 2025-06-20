#pragma once

#include <vector>
#include <string>

struct ProcessLogEntry {
    std::string timestamp;
    int coreID;
    std::string instruction;
};

class Process {

public:
    Process(const std::string& name, int numInstructions);

    std::string getName() const;
	int getRemainingInstruction() const;
    int getTotalInstructions() const;
    std::string getTimestamp() const;
	int getPID() const;
    int getCoreID();
	int setCoreID(int coreID);
	void executeInstruction();
	bool isComplete();

    std::vector<struct ProcessLogEntry> getLogs();

    // TEMPORARY for print function
    void logInstructionsToFile() const;

private:
    std::string name;
    int pid;
    int coreID = -1;
	int remainingInstruction;
    int totalInstructions;
    std::string timestamp;
    std::string generateTimestamp() const;

	std::vector<struct ProcessLogEntry> logEntries; // Vector to store log entries

	static int nextPID; // Static variable to keep track of the next PID
};
