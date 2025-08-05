#include "ReadInstruction.h"
#include "Process.h"
#include "ConsoleUtil.h"
#include "MemoryManager.h"

#include <sstream>
#include <iomanip>

// Constructor for ReadInstruction, initializes target variable and address (can be variable name or direct address)
ReadInstruction::ReadInstruction(const std::string& targetVar, std::variant<std::string, uint16_t> addr)
    : target(targetVar), address(addr) {}

// Executes the read instruction for the given process
int ReadInstruction::execute(Process& process) {
    uint32_t virtualAddress = 0;

    // Determine the virtual address: either from a variable or a direct value
    if (std::holds_alternative<std::string>(address)) {
        virtualAddress = process.getVariable(std::get<std::string>(address));
    } else {
        virtualAddress = std::get<uint16_t>(address);
    }

    // Check for memory violation (address out of bounds)
    if (virtualAddress + 1 >= process.getMemoryRequired()) {
        process.markTerminatedByMemoryViolation(virtualAddress);
        process.setState(ProcessState::Terminated);
        Process::unregisterProcess(process.getPID());
        return -1;
    }

    // Try to access memory (returns optional physical address)
    auto physicalAddrOpt = MemoryManager::getInstance()->accessMemory(process.shared_from_this(), virtualAddress, false);
    if (!physicalAddrOpt.has_value()) {
        // If memory access fails, block the process
        process.setState(ProcessState::Blocked);
        return -1;
    }

    // Read 16-bit value from the resolved physical address
    uint16_t value = MemoryManager::getInstance()->readUint16At(physicalAddrOpt.value());
    // Store the value in the process's variable table
    process.setVariable(target, value);

    // Log the read operation
    std::stringstream ss;
    ss << "READ\t\taddress 0x" << std::hex << virtualAddress
       << " with value " << std::dec << value
       << " and stored as " << target;

    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });
    return 0;
}

// Returns a string representation of the instruction
std::string ReadInstruction::toString() const {
    std::ostringstream oss;
    oss << "READ " << target;
    return oss.str();
}
