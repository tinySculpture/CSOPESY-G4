#include "AddInstruction.h"

AddInstruction::AddInstruction(const std::string& target,
    std::variant<std::string, uint16_t> op1,
    std::variant<std::string, uint16_t> op2)
    : target(target), op1(op1), op2(op2) {
}

int AddInstruction::execute(Process& process) {
	// Helper to resolve a variant operant to a uint16_t value
    auto resolve = [&](const std::variant<std::string, uint16_t>& v) -> uint16_t {
		// If operand is a variable name, fetch its current value from the process memory
        if (std::holds_alternative<std::string>(v))
            return process.getVariable(std::get<std::string>(v));
        // Otherwise, it's an immediate literal; return it immediately
        return std::get<uint16_t>(v);
        };

    // Resolve both operands
    uint16_t val1 = resolve(op1);
    uint16_t val2 = resolve(op2);

    // Perform addition and clamp to the maximum value representable by uint16_t
    uint16_t result = val1 + val2;
    result = std::min<uint32_t>(result, UINT16_MAX);

    // Store the result in the target variable within the process
    process.setVariable(target, result);

	// Log the operation
    std::stringstream ss;
    ss << "ADD\t\t" << target << " = " << val1 << " + " << val2 << " -> " << result;
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    return 0;
}

std::string AddInstruction::toString() const {
    return "ADD " + target;
}