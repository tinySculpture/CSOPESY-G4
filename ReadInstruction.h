#pragma once

#include "Instruction.h"
#include <string>
#include <variant>

/**
 * @class ReadInstruction
 * @brief Represents an instruction to read a value from a specified address into a target variable.
 *
 * This instruction reads data from a memory address (which can be specified as either a string or a uint16_t)
 * and stores the result in the given target variable within the process context.
 *
 * @constructor
 * @param targetVar The name of the variable to store the read value.
 * @param address The address to read from, specified as either a string (symbolic) or a uint16_t (direct address).
 *
 * @function execute
 * @brief Executes the read instruction on the given process.
 * @param process The process context in which to execute the instruction.
 * @return int Status code of execution.
 *
 * @function toString
 * @brief Returns a string representation of the instruction.
 * @return std::string The string representation.
 *
 * @private
 * @var target The target variable name.
 * @var address The address to read from (string or uint16_t).
 */
class ReadInstruction : public Instruction {
public:
    ReadInstruction(const std::string& targetVar, std::variant<std::string, uint16_t> address);
    int execute(Process& process) override;
    std::string toString() const override;

private:
    std::string target;
    std::variant<std::string, uint16_t> address;
};
