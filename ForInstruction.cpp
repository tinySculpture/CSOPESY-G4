#include <sstream>

#include "Process.h"
#include "ForInstruction.h"
#include "ConsoleUtil.h"

ForInstruction::ForInstruction(int loopCount, int layer)
    : loopCount(loopCount), layer(layer) {
}

void ForInstruction::addInstruction(const std::shared_ptr<Instruction>& instr) {
    if (instr) {
        innerInstructions.push_back(instr);
    }
}

std::string ForInstruction::buildNestedLog(const Process& process, int loopDepth) const {
    std::stringstream ss;
    ss << "FOR " << loopCount << " { ";

    for (const auto& instr : innerInstructions) {
        if (auto* nested = dynamic_cast<ForInstruction*>(instr.get())) {
            ss << nested->buildNestedLog(process, loopDepth + 1);
        }
        else {
            ss << instr->toString() << " ";
        }
    }

    ss << "} ";
    return ss.str();
}

int ForInstruction::execute(Process& process) {
    int pid = process.getPID();
    auto& state = processStates[pid];

    std::stringstream ss;
    ss << buildNestedLog(process, layer + 1);
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    if (state.counter >= loopCount) {
        processStates.erase(pid);
        return 0;
    }

    if (state.index < static_cast<int>(innerInstructions.size())) {
        auto& instr = innerInstructions[state.index];
        if (instr) {
            int delay = instr->execute(process);
            state.index++;
            return delay;
        }
    }

    if (state.index >= static_cast<int>(innerInstructions.size())) {
        state.index = 0;
        state.counter++;
    }

    return (state.counter < loopCount) ? 1 : 0;
}

std::string ForInstruction::toString() const {
    std::stringstream ss;
    ss << "FOR x" << loopCount << " [layer " << layer << "]";
    return ss.str();
}