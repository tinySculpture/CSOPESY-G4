#pragma once

#include <cstdint>

#include "Instruction.h"

class SleepInstruction : public Instruction {
public:
    explicit SleepInstruction(uint8_t ticks);
    int execute(Process& process) override;
    std::string toString() const override;

private:
    uint8_t ticks;
};