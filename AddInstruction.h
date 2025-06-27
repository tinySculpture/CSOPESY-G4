#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "Instruction.h"

/**
 * @class AddInstruction
 * @brief Concrete Instruction that adds two values (variables or literals).
 *
 * This instruction resolves its two operands (which may be named variables
 * or immediate uint16_t values) from the process context, sums them,
 * clamps the result to the uint16_t range, and stores it in the target variable.
 */
class AddInstruction : public Instruction {
public:
    /**
     * @brief Construct a new AddInstruction.
     *
     * @param target Name of the variable where the result will be stored.
     * @param op1     First operand: a variable name or an immediate value.
     * @param op2     Second operand: a variable name or an immediate value.
     */
    AddInstruction(const std::string& target,
        std::variant<std::string, uint16_t> op1,
        std::variant<std::string, uint16_t> op2);



    /**
     * @brief Execute the addition on the given process.
     *
     * Resolves each operand by fetching from process.memory if it's a string
     * or using the immediate value. Adds them together, clamps to uint16_t,
     * stores the result in the process, and logs the operation.
     *
     * @param process Reference to the executing Process.
	 * @return int    Number of delay ticks (Always 0 since an add instruction only needs one tick to execute)
     */
    int execute(Process& process) override;



    /**
     * @brief Returns a short string identifying this instruction for logs.
     *
     * @return std::string Always returns "ADD <target>".
     */
    std::string toString() const override;

private:
	std::string target;                             // Target variable name
	std::variant<std::string, uint16_t> op1, op2;   // Operands (variable names or immediate values)
};
