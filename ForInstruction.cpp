#include <sstream>

#include "Process.h"
#include "ForInstruction.h"
#include "ConsoleUtil.h"

ForInstruction::ForInstruction(std::vector<std::shared_ptr<Instruction>> body, int repeats)
    : body(std::move(body)), repeats(repeats) {
}

int ForInstruction::execute(Process& process) {
    return 0;
}

std::string ForInstruction::toString() const {
    return "FOR x" + std::to_string(repeats) + " { " + std::to_string(body.size()) + " instr }";
}
