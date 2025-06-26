#pragma once

#include <cstdint>
#include <string>

#include "Instruction.h"

class DeclareInstruction : public Instruction {
public:
    DeclareInstruction(const std::string& var, uint16_t value);
    int execute(Process& process) override;
    std::string toString() const override;

private:
    std::string var;
    uint16_t value;
};
