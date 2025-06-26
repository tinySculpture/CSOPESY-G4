#pragma once

#include "Instruction.h"

enum class PrintType {
    Hello, Literal, Variable
};

class PrintInstruction : public Instruction {
public:
    PrintInstruction(PrintType type, const std::string& data = "");
    int execute(Process& process) override;
    std::string toString() const override;

private:
    PrintType type;
    std::string data;
};
