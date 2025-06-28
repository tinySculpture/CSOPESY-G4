#include "ForInstruction.h"   

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
            // Recursively append nested loop representation
            ss << nested->buildNestedLog(process, loopDepth + 1);
        }
        else {
            // Append simple instruction label
            ss << instr->toString() << " ";
        }
    }

    ss << "} ";
    return ss.str();
}

int ForInstruction::execute(Process& process) {
    int pid = process.getPID();
    auto& state = processStates[pid];

    if (!state.initialized) {
        state.counter = 0;
        state.index = 0;
        state.initialized = true;
        state.hasLogged = false;
        state.delaying = false;
    }

    if (state.counter >= loopCount || innerInstructions.empty()) {
        processStates.erase(pid);
        return 0;
    }

    if (!state.hasLogged) {
        std::stringstream ss;
        ss << "[FOR loop " << (state.counter + 1) << "/" << loopCount << "] Block: { ";
        for (size_t i = 0; i < innerInstructions.size(); ++i) {
            ss << "I" << (i + 1) << ": " << innerInstructions[i]->toString();
            if (i != innerInstructions.size() - 1) ss << ", ";
        }
        ss << " }";
        process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });
        state.hasLogged = true;
    }

    if (state.delaying) {
        if (process.getDelayCounter() == 0) {
            state.delaying = false;
            state.index++;
            if (state.index >= static_cast<int>(innerInstructions.size())) {
                state.index = 0;
                state.counter++;
                state.hasLogged = false;
            }
        }
        return 0;
    }

    if (state.index < static_cast<int>(innerInstructions.size())) {
        auto& instr = innerInstructions[state.index];
        int delay = instr->execute(process);
        if (delay > 0) {
            state.delaying = true;
            return delay;
        }

        state.index++;
        if (state.index >= static_cast<int>(innerInstructions.size())) {
            state.index = 0;
            state.counter++;
            state.hasLogged = false;
        }
    }

    return 0;
}

std::string ForInstruction::toString() const {
    std::stringstream ss;
    ss << "FOR " << loopCount << " times";
    return ss.str();
}

bool ForInstruction::isComplete(int pid) const {
    auto it = processStates.find(pid);
    return it == processStates.end();
}