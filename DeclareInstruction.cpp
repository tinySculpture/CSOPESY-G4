#include <sstream>

#include "Process.h"
#include "DeclareInstruction.h"
#include "ConsoleUtil.h"

DeclareInstruction::DeclareInstruction(const std::string& var, uint16_t value)
    : var(var), value(value) {
}

int DeclareInstruction::execute(Process& process) {
    process.setVariable(var, value);

    std::stringstream ss;
    ss << "DECLARE\t" << var << " = " << value;
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    return 0;
}

std::string DeclareInstruction::toString() const {
    return "DECLARE " + var + " = " + std::to_string(value);
}
