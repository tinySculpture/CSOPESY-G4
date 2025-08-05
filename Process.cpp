#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Process.h"
#include "ConsoleUtil.h"
#include "SystemConfig.h"
#include "BackingStore.h"

#include "Instruction.h"

std::atomic<int> Process::nextPID{0};

Process::Process(const std::string& name, std::vector<std::shared_ptr<Instruction>> instructions, size_t memoryRequired)
    : name(name), instructions(std::move(instructions)), memoryRequired(memoryRequired) {
    pid = nextPID.fetch_add(1);
    creationTime = generateCreationTimestamp();
	numOfPages = (memoryRequired + SystemConfig::getInstance()->memoryPerFrame - 1) / SystemConfig::getInstance()->memoryPerFrame;
	pageTable.resize(numOfPages);


    // total number of variables to initialize (each assumed 2 bytes)
    size_t totalVars = memoryRequired / 2; // floor division

    uint32_t memPerFrame = SystemConfig::getInstance()->memoryPerFrame;
    uint32_t varsPerPage = memPerFrame / 2; // how many 2-byte vars fit in one frame/page
    if (varsPerPage == 0) {
        throw std::runtime_error("memoryPerFrame too small to hold any variable");
    }

    // Prepare empty symbol tables per page
    std::vector<std::vector<std::pair<std::string, uint16_t>>> perPageSymbolTables(numOfPages);

    // Distribute variables across pages
    for (size_t varIdx = 0; varIdx < numOfPages; ++varIdx) {
        perPageSymbolTables[varIdx].assign(
            memPerFrame,
            std::make_pair(std::string(), uint16_t{ 0 })
        );
    }

    // Write each page's symbol table to backing store (empty pages get written too if needed)
    for (size_t i = 0; i < numOfPages; ++i) {
        BackingStore::getInstance()->getInstance()->write_page(uint16_t(pid), i, perPageSymbolTables[i]);
    }
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

    // Execute and log
    int returnedDelay = instr->execute(*this);

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
    if (currentSymbolTable.find(var) == currentSymbolTable.end()) currentSymbolTable[var] = 0;
    return currentSymbolTable[var];
}

void Process::setVariable(const std::string& var, uint16_t value) {
    currentSymbolTable[var] = std::clamp<uint32_t>(value, 0, UINT16_MAX);
}

std::string Process::generateCreationTimestamp() const {
    return ConsoleUtil::generateTimestamp();
}

size_t Process::getMemoryRequired() const {
    return memoryRequired;
}

uint64_t Process::virtual_size() const {
    return memoryRequired;
}

size_t Process::num_pages() const {
    return numOfPages;
}

PageTableEntry& Process::page_table_at(size_t page_idx) {
	return pageTable.at(page_idx);
}

void Process::record_access_violation(const std::string& addr_hex) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ostringstream oss;
    oss << "Access violation at " << addr_hex << " in process " << pid;
    addLog({ConsoleUtil::generateTimestamp(), -1, oss.str()});

	m_had_violation = true;
	m_bad_address = addr_hex;
	m_violation_time = ConsoleUtil::generateTimestamp();
	state = ProcessState::Shutdown_Access_Violation; // Set state to indicate violation
}


std::string Process::get_violation_message() const {
    if (!m_had_violation) return "";
    std::ostringstream oss;
    oss << "Process " << name << " shut down due to memory access violation error that occurred at "
        << m_violation_time << ". " << m_bad_address << " invalid.";
    return oss.str();
}

void Process::clear_page_table() {
    pageTable.clear();
    numOfPages = 0;
}