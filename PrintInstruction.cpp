#include <iostream>
#include <sstream>

#include "Process.h"
#include "PrintInstruction.h"
#include "ConsoleUtil.h"

PrintInstruction::PrintInstruction(PrintType type, const std::string& data)
    : type(type), data(data) {
}

int PrintInstruction::execute(Process& process) {
    std::stringstream ss;
    switch (type) {
    case PrintType::Hello:
        ss << "PRINT\t\tHello World from " << process.getName() << "!";
        break;
    case PrintType::Literal:
        ss << "PRINT\t\t" << data;
        break;
    case PrintType::Variable: {
        auto val = process.getVariable(data);
        ss << "PRINT\t\tAccessing variable '" << data << "' with value " << std::to_string(val);
        break;
    }
    }
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });
    return 0;
}

std::string PrintInstruction::toString() const {
    return "PRINT (" + data + ")";
}
