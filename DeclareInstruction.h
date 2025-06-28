#pragma once

#include <cstdint>
#include <string>
#include <sstream>

#include "Process.h"
#include "Instruction.h"

/**
 * @class DeclareInstruction
 * @brief Concrete Instruction that declares a new variable
 *      in the process with a specified uint16_t value.
 *
 * This instruction sets the given variable name to the provided immediate value
 * within the process's memory and logs the action.
 */
class DeclareInstruction : public Instruction {
public:
    /**
     * @brief Construct a new DeclareInstruction.
     *
     * @param var   Name of the variable to declare or update.
     * @param value Initial uint16_t value to assign to the variable.
     */
    DeclareInstruction(const std::string& var, uint16_t value);



    /**
     * @brief Execute the declaration on the given process.
     *
     * Stores the immediate value into the process's variable map and logs the action.
     *
     * @param process Reference to the executing Process.
	 * @return int    Number of delay ticks (Always 0 since a declare instruction only needs one tick to execute)
     */
    int execute(Process& process) override;



    /**
     * @brief Returns a string representation of this instruction for logging.
     *
     * @return std::string Always returns "DECLARE <var> = <value>".
     */
    std::string toString() const override;

private:
	std::string var;    // name of the variable to declare
	uint16_t value;     // immediate value to assign to the variable
};
