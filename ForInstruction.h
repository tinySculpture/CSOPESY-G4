#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Instruction.h"

class ForInstruction : public Instruction {
public:
    ForInstruction(int loopCount, int layer);
    void addInstruction(const std::shared_ptr<Instruction>& instr);
    int execute(Process& process) override;
    std::string toString() const override;

private:
    struct LoopState {
        int counter = 0;
        int index = 0;
    };

    int loopCount;
    int layer;
    std::vector<std::shared_ptr<Instruction>> innerInstructions;

    mutable std::unordered_map<int, LoopState> processStates;

    std::string buildNestedLog(const Process& process, int loopDepth) const;
};
