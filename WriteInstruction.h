#pragma once

#include "Instruction.h"
#include <variant>
#include <string>

/**
 * @class WriteInstruction
 * @brief Represents an instruction to write a value to a specified address in a process.
 *
 * This instruction can target either a named variable (std::string) or a direct memory address (uint16_t),
 * and can write a value from either a named source or a direct value.
 *
 * @constructor
 * @param targetAddr The address or variable name to write to (std::variant<std::string, uint16_t>).
 * @param valueSrc The value or variable name to write from (std::variant<std::string, uint16_t>).
 *
 * @function execute
 * Executes the write instruction on the given process.
 * @param process The process on which to execute the instruction.
 * @return int Status code of execution.
 *
 * @function toString
 * Returns a string representation of the instruction.
 * @return std::string The string representation.
 *
 * @private
 * @var address The address or variable name to write to.
 * @var value The value or variable name to write from.
 */
class WriteInstruction : public Instruction {
public:
    WriteInstruction(std::variant<std::string, uint16_t> targetAddr, std::variant<std::string, uint16_t> valueSrc);
    int execute(Process& process) override;
    std::string toString() const override;

private:
    std::variant<std::string, uint16_t> address;
    std::variant<std::string, uint16_t> value;
};
