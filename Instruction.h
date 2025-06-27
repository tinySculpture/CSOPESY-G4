#pragma once

#include <memory>
#include <string>

class Process;

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual int execute(Process& process) = 0;
    virtual std::string toString() const = 0;
    virtual bool isComplete(int pid) const { return true; }
};