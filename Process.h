#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "MemoryManager.h"

class Instruction;

enum class ProcessState {
    Blocked,
    Ready,
    Running,
    Sleeping,
    Finished,
    Terminated
};

struct ProcessLogEntry {
    std::string timestamp;
    int coreID;
    std::string instruction;
};
class Process;

class Process : public std::enable_shared_from_this<Process> {
public:
    Process(const std::string& name, std::vector<std::shared_ptr<Instruction>> instructions, uint16_t mem, uint16_t pages);

    std::string getName() const; 
    int getPID() const;
	static int peakNextPID();
    std::string getCreationTime() const;
    unsigned long getDelayCounter() const;

    int getCoreID() const;               
    void setCoreID(int id);              

    ProcessState getState() const;       
    void setState(ProcessState state);   
    bool isFinished() const;  
    bool isTerminated() const;

    void executeInstruction(int delayPerExec);
    void tick();                         
    size_t getCurrentInstructionIndex() const; 
    size_t getRemainingInstruction() const;    
    size_t getTotalInstructions() const;      

    std::vector<ProcessLogEntry> getLogs() const;
    void addLog(const ProcessLogEntry& entry);

    uint16_t getVariable(const std::string& var);
    void setVariable(const std::string& var, uint16_t value);
    bool canDeclareVariable() const;

    uint32_t getMemoryRequired() const;
    void setMemoryRequired(uint32_t bytes);
    bool hasMinimumMemoryForVariables() const;

    uint32_t getPageCount() const;
    void setPageCount(uint32_t count);

    void initPageTable(uint32_t pageCount);
    std::vector<PageTableEntry>& getPageTable();

    void markTerminatedByMemoryViolation(uint32_t badAddress);
    
    std::string getTerminationTimestamp() const;
    uint32_t getInvalidMemoryAddress() const;
    bool isTerminatedByMemoryViolation() const;

    bool isWaitingOnPageFault() const { return waitingOnPageFault; }
    void setWaitingOnPageFault(bool waiting) { waitingOnPageFault = waiting; }
    static std::unordered_map<uint32_t, std::shared_ptr<Process>> pidToProcess;

    static void registerProcess(std::shared_ptr<Process> process);
    static void unregisterProcess(uint32_t pid);
    static std::shared_ptr<Process> getProcessByPID(uint32_t pid);

private:
    std::string name;                                           
    int pid;                                                   
    static std::atomic<int> nextPID;                            
    int coreID = -1;                                            
    std::string creationTime;                                   
    unsigned long delayCounter = 0;                            

    ProcessState state = ProcessState::Ready;                   
    std::unordered_map<std::string, uint16_t> symbolTable;
    
    uint32_t memoryRequired;
    uint32_t pageCount = 0;
    static constexpr uint32_t SYMBOL_TABLE_START = 0;
    static constexpr uint32_t SYMBOL_TABLE_MAX_VARS = 32;
    static constexpr uint32_t SYMBOL_TABLE_BYTES = 64;
    std::vector<PageTableEntry> pageTable;
    bool waitingOnPageFault = false;

    std::vector<std::shared_ptr<Instruction>> instructions;     
    size_t currentInstructionIndex = 0;                         

    mutable std::mutex logMutex;                                
    std::vector<ProcessLogEntry> logEntries;     
    
    std::string generateCreationTimestamp() const;

    bool terminatedDueToMemoryViolation = false;
    std::string terminationTimestamp;
    uint32_t invalidMemoryAddress = 0;
};