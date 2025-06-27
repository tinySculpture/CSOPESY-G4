#pragma once

#include "Instruction.h"

/**
 * @enum PrintType
 * @brief Types of print operations supported.
 *
 * - Hello:   Prints a default greeting with process name.
 * - Literal: Prints a custom literal message.
 * - Variable: Prints the value of a named variable.
 */
enum class PrintType {
    Hello,      // Default Greeting Message
    Literal,    // User-provided literal message
    Variable    // Value of a process variable
};

/**
 * @class PrintInstruction
 * @brief Concrete Instruction that logs a message to the process log.
 *
 * Depending on PrintType, it will log a greeting, a literal string, or
 * the current value of a variable retrieved from process memory.
 */
class PrintInstruction : public Instruction {
public:
    /**
     * @brief Construct a new PrintInstruction.
     *
     * @param type The category of print operation.
     * @param data Optional data: literal text or variable name.
     */
    PrintInstruction(PrintType type, const std::string& data = "");



    /**
     * @brief Execute the print operation for the given process.
     *
     * Formats a message based on PrintType, adds a log entry with timestamp
     * and core ID, then returns 0 delay ticks.
     *
     * @param process Reference to the executing Process.
     * @return int Number of delay ticks (Always 0 since a print instruction only needs one tick to execute)
     */
    int execute(Process& process) override;



    /**
     * @brief Provides a short identifier for this instruction.
     *
     * @return std::string e.g., "PRINT (data)".
     */
    std::string toString() const override;

private:
    PrintType type;     // Kind of print to perform
    std::string data;   // Literal or variable name
};
