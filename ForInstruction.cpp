﻿#include <sstream>

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

    // Initialize state on first entry
    if (!state.initialized) {
        state.counter = 0;
        state.index = 0;
        state.initialized = true;
        state.hasLogged = false;
        state.delaying = false;
    }

    // If loop has finished or no inner instructions, clean up
    if (state.counter >= loopCount || innerInstructions.empty()) {
        processStates.erase(pid);
        return 0;
    }

    // Log iteration header once per loop iteration
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

    // Handle waiting out a delay from a previous instruction
    if (state.delaying) {
        if (process.getDelayCounter() == 0) {
            state.delaying = false;
            state.index++;
            // If iteration complete, reset and advance counter
            if (state.index >= static_cast<int>(innerInstructions.size())) {
                state.index = 0;
                state.counter++;
                state.hasLogged = false;
            }
        }
        return 0;
    }

    // Execute next inner instruction if pending
    if (state.index < static_cast<int>(innerInstructions.size())) {
        auto& instr = innerInstructions[state.index];
        int delay = instr->execute(process);
        if (delay > 0) {
            // Mark as delaying and return delay duration
            state.delaying = true;
            return delay;
        }
        // No delay: advance index and possibly new iteration
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