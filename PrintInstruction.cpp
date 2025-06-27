#include <iostream>
#include <sstream>

#include "Process.h"
#include "PrintInstruction.h"
#include "ConsoleUtil.h"

PrintInstruction::PrintInstruction(PrintType type, const std::string& data)
    : type(type), data(data) {
}

int PrintInstruction::execute(Process& process) {
    // Build the log message depending on the print type
    std::stringstream ss;
    switch (type) {
        case PrintType::Hello:
            // Default greeting includes process name
            ss << "PRINT\t\tHello World from " << process.getName() << "!";
            break;
        case PrintType::Literal:
            // Literal Message provided by the instruction
            ss << "PRINT\t\t" << data;
            break;
        case PrintType::Variable: {
            // Fetch variable value from process memory
            auto val = process.getVariable(data);
            ss << "PRINT\t\tAccessing variable '" << data << "' with value " << std::to_string(val);
            break;
        }
    }
    // Add the constructed message to the process log with timestamp and core ID
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    // Return 0 to indicate no extra delay ticks
    return 0;
}

std::string PrintInstruction::toString() const {
    return "PRINT (" + data + ")";
}
