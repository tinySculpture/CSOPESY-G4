#pragma once
#include <memory>
#include <string>
#include "Instruction.h"

/**
 * @class InstructionParser
 * @brief Provides functionality to parse instruction strings into Instruction objects.
 *
 * The InstructionParser class offers a static method to convert a line of text
 * representing an instruction into a corresponding Instruction object.
 */
class InstructionParser {
public:
    /**
     * @brief Parses a line of text into an Instruction object.
     *
     * This static method takes a string representing a single instruction and
     * attempts to parse it into an Instruction object. If parsing is successful,
     * a shared pointer to the Instruction is returned; otherwise, the behavior
     * depends on the implementation (e.g., may throw an exception or return nullptr).
     *
     * @param line The string containing the instruction to parse.
     * @return std::shared_ptr<Instruction> A shared pointer to the parsed Instruction object.
     */
    static std::shared_ptr<Instruction> parse(const std::string& line);
};