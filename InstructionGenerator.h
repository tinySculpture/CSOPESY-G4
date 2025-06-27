#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "Instruction.h"
#include "SystemConfig.h"

/**
 * @class InstructionGenerator
 * @brief Factory for creating a vector of Instructions for a given process ID.
 *
 * Uses a PRNG to generate a random count of instructions within configured bounds,
 * tracks declared variables to ensure valid operands, and supports nested loops up to
 * a maximum depth.
 */
class InstructionGenerator {
public:
    /**
     * @brief Generate a sequence of instructions for a new process.
     *
	 * @param pid       Unique process ID for which to generate instructions.
     * @param config    SystemConfig containing min/max instructions.
     * @return std::vector<std::shared_ptr<Instruction>> Randomized instruction list.
     */
    static std::vector<std::shared_ptr<Instruction>> generateInstructions(int pid, const SystemConfig& config);

private:
    /**
     * @brief Create a single random instruction, ensuring variable scope correctness.
     *
	 * @param pid           Process ID for which to create the instruction.
     * @param declaredVars  Set of variable names already declared.
     * @param config        SystemConfig for instruction count bounds.
     * @param layer         Current nesting depth for FOR loops.
     * @return std::shared_ptr<Instruction> Newly created instruction.
     */
    static std::shared_ptr<Instruction> randomInstruction(int pid, std::unordered_set<std::string>& declaredVars, const SystemConfig& config, int layer = 0);

	/// Generate a random variable name that is not already declared.
    static std::string randomVarName();

	/// Generate a random uint16_t in [0,500].
    static uint16_t randomUint16();

	/// Generate a random sleep duration in ticks [1,5].
    static uint8_t randomSleepDuration();

    /// Format a randomized print message including the process ID.
    static std::string randomPrintMessage(int pid);
};