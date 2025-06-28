#include <algorithm>
#include <sstream>

#include "Process.h"
#include "SubtractInstruction.h"
#include "ConsoleUtil.h"

SubtractInstruction::SubtractInstruction(const std::string& target,
    std::variant<std::string, uint16_t> op1,
    std::variant<std::string, uint16_t> op2)
    : target(target), op1(op1), op2(op2) {
}

int SubtractInstruction::execute(Process& process) {
    // Helper lambda to resolve a variable name or literal to a uint16_t value
    auto resolve = [&](const std::variant<std::string, uint16_t>& v) -> uint16_t {
        // If operand is a variable name, fetch its value from process memory
        if (std::holds_alternative<std::string>(v))
            return process.getVariable(std::get<std::string>(v));

        // Otherwise, it's an immediate literal; return it directly
        return std::get<uint16_t>(v);
        };

    // Resolve both operands
    uint16_t val1 = resolve(op1);
    uint16_t val2 = resolve(op2);

    // Perform subtraction; since unsigned, clamp overflow to UINT16_MAX
    uint16_t result = val1 - val2;
    result = std::min<uint32_t>(result, UINT16_MAX);

    // Store the result in the target variable within the process
    process.setVariable(target, result);

    // // Build a descriptive log entry
    std::stringstream ss;
    ss << "SUBTRACT\t" << target << " = " << val1 << " - " << val2 << " -> " << result;
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    // Return 0 to indicate no extra delay ticks
    return 0;
}

std::string SubtractInstruction::toString() const {
    return "SUBTRACT " + target;
}
