#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class Instruction;

/**
 * @enum ProcessState
 * @brief Lifecycle states of a Process.
 */
enum class ProcessState {
    Ready,      // Waiting in the scheduler queue
    Running,    // Currently executing on a Core
    Sleeping,   // Delaying before next execution tick
    Finished    // Completed all instructions
};



/**
 * @struct ProcessLogEntry
 * @brief Single log entry for instruction execution tracking.
 */
struct ProcessLogEntry {
    std::string timestamp;      // Timestamp of the log entry.
    int coreID;                 // Core that executed or delayed the instruction
    std::string instruction;    // Textual description of the executed instruction
};



/**
 * @class Process
 * @brief Encapsulates a list of Instructions and manages execution state.
 *
 * Handles PID assignment, instruction sequencing, delays, variable memory,
 * and capturing execution logs with thread-safety.
 */
class Process {
public:
    /**
     * @brief Construct a Process with a name and instruction sequence.
     * @param name        Human-readable process name.
     * @param instructions Vector of Instruction instances to execute.
     */
    Process(const std::string& name, std::vector<std::shared_ptr<Instruction>> instructions);



    /** @name Metadata Accessors */
    ///@{
    /** @return Process name. */
    std::string getName() const; 

    /** @return Unique process ID. */
    int getPID() const;

    /** @return Next PID to be assigned. */
	static int peakNextPID();

    /** @return Human-readable creation timestamp. */
    std::string getCreationTime() const;
    ///@}




    /** @name Core Affinity */
    ///@{
    /** @return ID of assigned core or -1. */
    int getCoreID() const;               

    /** @param id Core identifier or -1 for none. */
    void setCoreID(int id);              
    ///@}



    /** @name State Management */
    ///@{
    /** @return Current lifecycle state. */
    ProcessState getState() const;       

    /** @param state New ProcessState. */
    void setState(ProcessState state);   

    /** @return True if all instructions executed. */
    bool isFinished() const;             
    ///@}



    /** @name Instruction Sequencing */
    ///@{
    /**
     * Execute or delay a single step:
     * - If sleeping, decrement delayCounter.
     * - Else, run one instruction, log it, and reset delayCounter.
     * 
     * @param delayPerExec delay between instructions
     */
    void executeInstruction(int delayPerExec);

    /** @brief Decrement delayCounter if >0. */
    void tick();                         

    /** @return Index of current instruction. */
    size_t getCurrentInstructionIndex() const; 

    /** @return Count left to execute. */
    size_t getRemainingInstruction() const;    

    /** @return Total instruction count. */
    size_t getTotalInstructions() const;       
    ///@}



    /** @name Logging */
    ///@{
    /** @return Thread-safe copy of all log entries. */
    std::vector<ProcessLogEntry> getLogs() const;

    /** @param entry Log to append (thread-safe). */
    void addLog(const ProcessLogEntry& entry);
    ///@}



    /** @name Variable Memory */
    ///@{
    /** @param var Name of the variable.
     *   @return Current value (0 if undeclared).
     */
    uint16_t getVariable(const std::string& var);

    /** @param var Name 
     *  @param value to assign (clamped).
     */
    void setVariable(const std::string& var, uint16_t value);
    ///@}

private:
    std::string name;                                           // Human-readable name
    int pid;                                                    // Unique process ID
    static std::atomic<int> nextPID;                            // reference in generating unique PIDs for new processes
    int coreID = -1;                                            // ID of assigned core
    std::string creationTime;                                   // Timestamp of creation
    unsigned long delayCounter = 0;                             // Ticks remaining before execution of next instruction

    ProcessState state = ProcessState::Ready;                   // Initial state
    std::unordered_map<std::string, uint16_t> memory;           // Variable storage

    std::vector<std::shared_ptr<Instruction>> instructions;     // Sequence to execute
    size_t currentInstructionIndex = 0;                         // Current instruction index

    mutable std::mutex logMutex;                                // Guards logEntries
    std::vector<ProcessLogEntry> logEntries;                    // Recorded execution logs

    /** @return Formatted timestamp. */
    std::string generateCreationTimestamp() const;
};