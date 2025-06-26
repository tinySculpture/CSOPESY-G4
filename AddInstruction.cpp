#include <algorithm>
#include <sstream>

#include "Process.h"
#include "AddInstruction.h"
#include "ConsoleUtil.h"

AddInstruction::AddInstruction(const std::string& target,
    std::variant<std::string, uint16_t> op1,
    std::variant<std::string, uint16_t> op2)
    : target(target), op1(op1), op2(op2) {
}

int AddInstruction::execute(Process& process) {
    auto resolve = [&](const std::variant<std::string, uint16_t>& v) -> uint16_t {
        if (std::holds_alternative<std::string>(v))
            return process.getVariable(std::get<std::string>(v));
        return std::get<uint16_t>(v);
        };

    uint16_t val1 = resolve(op1);
    uint16_t val2 = resolve(op2);
    uint16_t result = val1 + val2;
    result = std::min<uint32_t>(result, UINT16_MAX);
    process.setVariable(target, result);

    std::stringstream ss;
    ss << "ADD\t\t" << target << " = " << val1 << " + " << val2 << " -> " << result;
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    return 0;
}

std::string AddInstruction::toString() const {
    return "ADD " + target;
}
