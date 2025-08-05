#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "Instruction.h"
#include "SystemConfig.h"

/**
 * @class InstructionGenerator
 * @brief Utility class for generating and parsing instructions for processes.
 *
 * This class provides static methods to generate random instructions for a process,
 * create random variable names and values, and parse custom instruction strings.
 */
class InstructionGenerator {
public:
    /**
     * @brief Generates a sequence of instructions for a given process.
     * @param pid The process ID for which instructions are generated.
     * @param config The system configuration reference.
     * @param allocatedMemory The amount of memory allocated to the process.
     * @return A vector of shared pointers to generated Instruction objects.
     */
    static std::vector<std::shared_ptr<Instruction>> generateInstructions(int pid, const SystemConfig& config, uint16_t allocatedMemory);

private:
    /**
     * @brief Generates a random instruction for a process.
     * @param pid The process ID.
     * @param declaredVars Set of already declared variable names.
     * @param addressVars Map of variable names to their memory addresses.
     * @param config The system configuration reference.
     * @param allocatedMemory The amount of memory allocated to the process.
     * @param layer The current recursion layer (default is 0).
     * @return A shared pointer to the generated Instruction.
     */
    static std::shared_ptr<Instruction> randomInstruction(int pid, std::unordered_set<std::string>& declaredVars, std::unordered_map<std::string, uint16_t>& addressVars, const SystemConfig& config, uint16_t allocatedMemory, int layer = 0);

    /**
     * @brief Generates a random variable name.
     * @return A randomly generated variable name as a string.
     */
    static std::string randomVarName();

    /**
     * @brief Generates a random 16-bit unsigned integer.
     * @return A randomly generated uint16_t value.
     */
    static uint16_t randomUint16();

    /**
     * @brief Generates a random sleep duration.
     * @return A randomly generated uint8_t value representing sleep duration.
     */
    static uint8_t randomSleepDuration();

    /**
     * @brief Generates a random print message for a process.
     * @param pid The process ID.
     * @return A randomly generated print message as a string.
     */
    static std::string randomPrintMessage(int pid);

    /**
     * @brief Parses a list of raw instruction strings into Instruction objects.
     * @param rawInstructions A vector of raw instruction strings.
     * @return A vector of shared pointers to parsed Instruction objects.
     */
    static std::vector<std::shared_ptr<Instruction>> parseCustomInstructions(const std::vector<std::string>& rawInstructions);
};