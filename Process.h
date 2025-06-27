#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Instruction.h"

enum class ProcessState {
    Ready, Running, Sleeping, Finished
};

struct ProcessLogEntry {
    std::string timestamp;
    int coreID;
    std::string instruction;
};

class Process {
public:
    Process(const std::string& name, std::vector<std::shared_ptr<Instruction>> instructions);

    std::string getName() const;
    int getPID() const;
	static int peakNextPID();
    std::string getCreationTime() const;
    int getCoreID() const;
    void setCoreID(int id);

    ProcessState getState() const;
    void setState(ProcessState state);
    unsigned long getDelayCounter() const;

    size_t getCurrentInstructionIndex() const;
    size_t getRemainingInstruction() const;
    size_t getTotalInstructions() const;

    void executeInstruction(int delayPerExec);
    void tick();

    bool isFinished() const;

    std::vector<ProcessLogEntry> getLogs() const;
    void addLog(const ProcessLogEntry& entry);

    uint16_t getVariable(const std::string& var);
    void setVariable(const std::string& var, uint16_t value);

private:
    std::string name;
    int pid;
    static std::atomic<int> nextPID;
    int coreID = -1;
    std::string creationTime;
    unsigned long delayCounter = 0;

    ProcessState state = ProcessState::Ready;
    std::unordered_map<std::string, uint16_t> memory;

    std::vector<std::shared_ptr<Instruction>> instructions;
    size_t currentInstructionIndex = 0;

    mutable std::mutex logMutex;
    std::vector<ProcessLogEntry> logEntries;

    std::string generateCreationTimestamp() const;
};