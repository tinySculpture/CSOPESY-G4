#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "Instruction.h"

class SubtractInstruction : public Instruction {
public:
    SubtractInstruction(const std::string& target,
        std::variant<std::string, uint16_t> op1,
        std::variant<std::string, uint16_t> op2);

    int execute(Process& process) override;
    std::string toString() const override;

private:
    std::string target;
    std::variant<std::string, uint16_t> op1, op2;
};