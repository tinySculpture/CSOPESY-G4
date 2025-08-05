#include "Instruction.h"
#include "AddInstruction.h"
#include "DeclareInstruction.h"
#include "PrintInstruction.h"
#include "ReadInstruction.h"
#include "WriteInstruction.h"
#include "ConsoleUtil.h"

#include <sstream>
#include <regex>
#include <stdexcept>
#include <cctype>
#include <algorithm>

static std::variant<std::string, uint16_t> parseOperand(const std::string& token) {
    if ((token.rfind("0x", 0) == 0) || std::all_of(token.begin(), token.end(), ::isdigit)) {
        return static_cast<uint16_t>(std::stoul(token, nullptr, 0));
    }
    return token;
}

/**
 * @brief Parses a single line of text and constructs the corresponding Instruction object.
 *
 * This static factory method analyzes the given string, determines the instruction type
 * (e.g., DECLARE, ADD, WRITE, READ, PRINT), and creates an appropriate Instruction-derived
 * object with parsed arguments.
 *
 * Supported instructions:
 * - DECLARE <var> <value>: Declares a variable with a given value.
 * - ADD <target> <op1> <op2>: Adds two operands and stores the result in target.
 * - WRITE <addr> <val>: Writes a value to a memory address.
 * - READ <target> <addr>: Reads a value from a memory address into target.
 * - PRINT(...): Prints a literal, variable, expression, or "Hello".
 *
 * @param line The input string representing a single instruction line.
 * @return std::shared_ptr<Instruction> A shared pointer to the constructed Instruction object.
 * @throws std::runtime_error If the instruction type is unknown or the syntax is invalid.
 *
 * Example usage:
 *   auto instr = Instruction::fromString("DECLARE x 10");
 *
 * Inline documentation:
 * - Extracts the first word as the instruction keyword.
 * - For each supported keyword, parses the rest of the line into appropriate arguments.
 * - For PRINT, handles parentheses, string literals, expressions, and variables.
 * - Throws if the instruction is unrecognized or the syntax is malformed.
 */
std::shared_ptr<Instruction> Instruction::fromString(const std::string& line) {
    std::istringstream iss(line);
    std::string keyword;
    iss >> keyword;

    if (keyword == "DECLARE") {
        std::string var;
        uint16_t value;
        iss >> var >> value;
        return std::make_shared<DeclareInstruction>(var, value);
    }

    if (keyword == "ADD") {
        std::string target, op1, op2;
        iss >> target >> op1 >> op2;
        return std::make_shared<AddInstruction>(target, parseOperand(op1), parseOperand(op2));
    }

    if (keyword == "WRITE") {
        std::string addr, val;
        iss >> addr >> val;
        return std::make_shared<WriteInstruction>(parseOperand(addr), parseOperand(val));
    }

    if (keyword == "READ") {
        std::string target, addr;
        iss >> target >> addr;
        return std::make_shared<ReadInstruction>(target, parseOperand(addr));
    }

    if (keyword == "PRINT") {
        std::string rest;
        std::getline(iss, rest);
        rest = ConsoleUtil::trim(rest);

        if (rest.front() != '(' || rest.back() != ')') {
            throw std::runtime_error("Invalid PRINT syntax: " + line);
        }

        rest = rest.substr(1, rest.length() - 2); 
        rest = ConsoleUtil::trim(rest);

        if (rest == "\"Hello\"") {
            return std::make_shared<PrintInstruction>(PrintType::Hello, "");
        }

        if (rest.front() == '"' && rest.back() == '"') {
            return std::make_shared<PrintInstruction>(PrintType::Literal, rest.substr(1, rest.length() - 2));
        }

        if (rest.find('+') != std::string::npos) {
            return std::make_shared<PrintInstruction>(PrintType::Expression, rest);
        }

        return std::make_shared<PrintInstruction>(PrintType::Variable, rest);
    }

    throw std::runtime_error("Unknown instruction type: " + keyword);
}