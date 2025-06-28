#include "DeclareInstruction.h"

DeclareInstruction::DeclareInstruction(const std::string& var, uint16_t value)
    : var(var), value(value) {
}

int DeclareInstruction::execute(Process& process) {
    // Set the variable in the process memory to the specified value
    process.setVariable(var, value);

    // Log the declaration action with timestamp and core ID
    std::stringstream ss;
    ss << "DECLARE\t" << var << " = " << value;
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    return 0;
}

std::string DeclareInstruction::toString() const {
    return "DECLARE " + var + " = " + std::to_string(value);
}
