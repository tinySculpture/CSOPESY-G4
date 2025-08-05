#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Process.h"
#include "ConsoleUtil.h"

#include "Instruction.h"

std::atomic<int> Process::nextPID{0};
std::unordered_map<uint32_t, std::shared_ptr<Process>> Process::pidToProcess;

Process::Process(const std::string& name, std::vector<std::shared_ptr<Instruction>> instructions, uint16_t mem, uint16_t pages)
    : name(name), instructions(std::move(instructions)), memoryRequired(mem), pageCount(pages) {
    pid = nextPID.fetch_add(1);
    creationTime = generateCreationTimestamp();
}

std::string Process::getName() const { return name; }
int Process::getPID() const { return pid; }
int Process::peakNextPID() { return nextPID.load(); }
std::string Process::getCreationTime() const { return creationTime; }
int Process::getCoreID() const { return coreID; }
void Process::setCoreID(int id) { coreID = id; }
unsigned long Process::getDelayCounter() const { return delayCounter; }

ProcessState Process::getState() const { return state; }
void Process::setState(ProcessState s) { state = s; }

size_t Process::getCurrentInstructionIndex() const { return currentInstructionIndex; }
size_t Process::getRemainingInstruction() const { return instructions.size() - currentInstructionIndex; }
size_t Process::getTotalInstructions() const { return instructions.size(); }

void Process::executeInstruction(int delayPerExec) {
    if (getRemainingInstruction() == 0) return;

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

    int returnedDelay = instr->execute(*this);

    if (state == ProcessState::Terminated) return;

    if (instr->isComplete(pid)) {
        currentInstructionIndex++;
    }

    delayCounter = std::max(delayPerExec, returnedDelay);
}

void Process::tick() {
    if (delayCounter > 0)
        delayCounter--;
}

bool Process::isFinished() const {
    return state == ProcessState::Finished;
}

bool Process::isTerminated() const {
    return state == ProcessState::Terminated;
}

std::vector<ProcessLogEntry> Process::getLogs() const {
    std::lock_guard<std::mutex> lock(logMutex);
    return logEntries;
}

void Process::addLog(const ProcessLogEntry& entry) {
    std::lock_guard<std::mutex> lock(logMutex);
    logEntries.push_back(entry);
}

std::string Process::generateCreationTimestamp() const {
    return ConsoleUtil::generateTimestamp();
}

uint16_t Process::getVariable(const std::string& var) {
    auto it = symbolTable.find(var);
    if (it == symbolTable.end()) return 0;
    return it->second;
}

void Process::setVariable(const std::string& var, uint16_t value) {
    if (!hasMinimumMemoryForVariables()) return;
    if (symbolTable.find(var) == symbolTable.end()) {
        if (symbolTable.size() >= SYMBOL_TABLE_MAX_VARS) return;
    }
    symbolTable[var] = std::clamp<uint32_t>(value, 0, UINT16_MAX);
}

bool Process::canDeclareVariable() const {
    return hasMinimumMemoryForVariables() && symbolTable.size() < SYMBOL_TABLE_MAX_VARS;
}

uint32_t Process::getMemoryRequired() const {
    return memoryRequired;
}

void Process::setMemoryRequired(uint32_t bytes) {
    memoryRequired = bytes;
}

bool Process::hasMinimumMemoryForVariables() const {
    return memoryRequired >= SYMBOL_TABLE_BYTES;
}

uint32_t Process::getPageCount() const {
    return pageCount;
}

void Process::setPageCount(uint32_t count) {
    pageCount = count;
}

void Process::initPageTable(uint32_t pageCount) {
    pageTable.resize(pageCount); 
}

std::vector<PageTableEntry>& Process::getPageTable() {
    return pageTable;
}

void Process::markTerminatedByMemoryViolation(uint32_t badAddress) {
    terminatedDueToMemoryViolation = true;
    terminationTimestamp = ConsoleUtil::generateTimestamp(); 
    invalidMemoryAddress = badAddress;
}

bool Process::isTerminatedByMemoryViolation() const { return terminatedDueToMemoryViolation; }
std::string Process::getTerminationTimestamp() const { return terminationTimestamp; }
uint32_t Process::getInvalidMemoryAddress() const { return invalidMemoryAddress; }

void Process::registerProcess(std::shared_ptr<Process> process) {
    pidToProcess[process->getPID()] = process;
}

void Process::unregisterProcess(uint32_t pid) {
    pidToProcess.erase(pid);
}

std::shared_ptr<Process> Process::getProcessByPID(uint32_t pid) {
    auto it = pidToProcess.find(pid);
    return (it != pidToProcess.end()) ? it->second : nullptr;
}