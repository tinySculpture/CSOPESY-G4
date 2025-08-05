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
        case PrintType::Expression: {
            std::stringstream output;
            std::string expr = data;
            size_t pos = 0;

            while ((pos = expr.find('+')) != std::string::npos) {
                std::string part = ConsoleUtil::trim(expr.substr(0, pos));
                expr = expr.substr(pos + 1);

                if (!part.empty()) {
                    if (part.front() == '"' && part.back() == '"') {
                        output << part.substr(1, part.length() - 2);
                    } else {
                        output << process.getVariable(part);
                    }
                }
            }
            expr = ConsoleUtil::trim(expr);
            if (!expr.empty()) {
                if (expr.front() == '"' && expr.back() == '"') {
                    output << expr.substr(1, expr.length() - 2);
                } else {
                    output << process.getVariable(expr);
                }
            }

            ss << "PRINT\t\t" << output.str();
            break;
        }
    }
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });
    
    return 0;
}

std::string PrintInstruction::toString() const {
    return "PRINT (" + data + ")";
}
