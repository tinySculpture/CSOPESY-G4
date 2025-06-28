#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "Instruction.h"

/**
 * @class SubtractInstruction
 * @brief Concrete Instruction that subtracts two values (variables or literals).
 *
 * This instruction resolves its two operands (which may be named variables
 * or immediate uint16_t values) from the process context, subtracts the second
 * from the first, clamps the result to the uint16_t range, and stores it in
 * the target variable.
 */
class SubtractInstruction : public Instruction {
public:
    /**
     * @brief Construct a new SubtractInstruction.
     *
     * @param target Name of the variable where the result will be stored.
     * @param op1     Minuend: a variable name or an immediate value.
     * @param op2     Subtrahend: a variable name or an immediate value.
     */
    SubtractInstruction(const std::string& target,
        std::variant<std::string, uint16_t> op1,
        std::variant<std::string, uint16_t> op2);



    /**
     * @brief Execute the subtraction on the given process.
     *
     * Resolves each operand by fetching from process memory if it's a string
     * or using the immediate value. Subtracts them, clamps to uint16_t,
     * stores the result in the process, and logs the operation.
     *
     * @param process Reference to the executing Process.
     * @return int    Delay ticks to apply after execution (0 for no extra delay).
     */
    int execute(Process& process) override;



    /**
     * @brief Returns a short string identifying this instruction for logs.
     *
     * @return std::string Always returns "SUBTRACT <target>".
     */
    std::string toString() const override;

private:
    std::string target;                             // target variable name
    std::variant<std::string, uint16_t> op1, op2;   // operands
};