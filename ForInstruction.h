#pragma once

#include <vector>

#include "Instruction.h"

class ForInstruction : public Instruction {
public:
    ForInstruction(std::vector<std::shared_ptr<Instruction>> body, int repeats);
    int execute(Process& process) override;
    std::string toString() const override;

private:
    std::vector<std::shared_ptr<Instruction>> body;
    int repeats;
    int current = 0;
    size_t innerIndex = 0;
};