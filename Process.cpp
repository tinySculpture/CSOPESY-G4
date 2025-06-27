#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Process.h"
#include "ConsoleUtil.h"

std::atomic<int> Process::nextPID{0};

Process::Process(const std::string& name, std::vector<std::shared_ptr<Instruction>> instructions)
    : name(name), instructions(std::move(instructions)) {
    pid = nextPID.fetch_add(1);
    creationTime = generateCreationTimestamp();
}

std::string Process::getName() const { return name; }
int Process::getPID() const { return pid; }
int Process::peakNextPID() { return nextPID.load(); }
std::string Process::getCreationTime() const { return creationTime; }
int Process::getCoreID() const { return coreID; }
void Process::setCoreID(int id) { coreID = id; }

ProcessState Process::getState() const { return state; }
void Process::setState(ProcessState s) { state = s; }

size_t Process::getCurrentInstructionIndex() const {
    return currentInstructionIndex;
}

size_t Process::getRemainingInstruction() const {
    return instructions.size() - currentInstructionIndex;
}

size_t Process::getTotalInstructions() const {
    return instructions.size();
}

void Process::executeInstruction(int delayPerExec) {
    if (isFinished()) return;

    if (delayCounter > 0) {
        setState(ProcessState::Sleeping);
        delayCounter--;
        return;
    }

    setState(ProcessState::Running);

    auto& instr = instructions[currentInstructionIndex];

    {
        std::lock_guard<std::mutex> lock(logMutex);
    }

    // Execute and log
    ++currentInstructionIndex;
    int returnedDelay = instr->execute(*this);
    delayCounter = std::max(delayPerExec, returnedDelay);

    if (isFinished()) {
        state = ProcessState::Finished;
        setCoreID(-1);
    }
}

void Process::tick() {
    if (delayCounter > 0)
        delayCounter--;
}

bool Process::isFinished() const {
    return currentInstructionIndex >= instructions.size();
}

std::vector<ProcessLogEntry> Process::getLogs() const {
    std::lock_guard<std::mutex> lock(logMutex);
    return logEntries;
}

void Process::addLog(const ProcessLogEntry& entry) {
    std::lock_guard<std::mutex> lock(logMutex);
    logEntries.push_back(entry);
}

uint16_t Process::getVariable(const std::string& var) {
    // Auto-declare variable with default 0
    if (memory.find(var) == memory.end()) memory[var] = 0;
    return memory[var];
}

void Process::setVariable(const std::string& var, uint16_t value) {
    memory[var] = std::clamp<uint32_t>(value, 0, UINT16_MAX);
}

std::string Process::generateCreationTimestamp() const {
    return ConsoleUtil::generateTimestamp();
}