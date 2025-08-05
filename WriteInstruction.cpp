#include "WriteInstruction.h"
#include "Process.h"
#include "ConsoleUtil.h"
#include "MemoryManager.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

// Constructor: initializes address and value (can be string or uint16_t)
WriteInstruction::WriteInstruction(std::variant<std::string, uint16_t> addr, std::variant<std::string, uint16_t> val)
    : address(addr), value(val) {}

// Executes the write instruction for the given process
int WriteInstruction::execute(Process& process) {
    uint32_t virtualAddress = 0;
    uint16_t valueToWrite = 0;

    // Resolve address: if string, get variable value; else use uint16_t directly
    if (std::holds_alternative<std::string>(address))
        virtualAddress = process.getVariable(std::get<std::string>(address));
    else
        virtualAddress = std::get<uint16_t>(address);

    // Check for memory violation (address out of bounds)
    if (virtualAddress + 1 >= process.getMemoryRequired()) {
        process.markTerminatedByMemoryViolation(virtualAddress);
        process.setState(ProcessState::Terminated);
        Process::unregisterProcess(process.getPID());
        return -1;
    }

    // Resolve value: if string, get variable value; else use uint16_t directly
    if (std::holds_alternative<std::string>(value))
        valueToWrite = process.getVariable(std::get<std::string>(value));
    else
        valueToWrite = std::get<uint16_t>(value);

    // Try to access memory (get physical address)
    auto physicalAddrOpt = MemoryManager::getInstance()->accessMemory(process.shared_from_this(), virtualAddress, true);
    if (!physicalAddrOpt.has_value()) {
        process.setState(ProcessState::Blocked); // Block if memory not available
        return -1;
    }

    // Write the value to the resolved physical address
    MemoryManager::getInstance()->writeUint16At(physicalAddrOpt.value(), valueToWrite);

    // Log the write operation
    std::stringstream ss;
    ss << "WRITE\t\tvalue " << valueToWrite << " to address 0x"
       << std::hex << virtualAddress;

    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });
    return 0;
}

// Returns a string representation of the instruction
std::string WriteInstruction::toString() const {
    std::ostringstream oss;
    oss << "WRITE ";
    if (std::holds_alternative<std::string>(value))
        oss << std::get<std::string>(value);
    else
        oss << std::get<uint16_t>(value);
    return oss.str();
}