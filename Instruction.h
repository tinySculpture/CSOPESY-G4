﻿#pragma once

#include <memory>
#include <string>

class Process;

/**
 * @class Instruction
 * @brief Interface for CPU-like instructions within a process emulator.
 *
 * Each Instruction subclass encapsulates a specific operation
 * and must implement execution logic and a human-readable label.
 */
class Instruction {
public:
    virtual ~Instruction() = default;

    virtual int execute(Process& process) = 0;
    virtual std::string toString() const = 0;
};