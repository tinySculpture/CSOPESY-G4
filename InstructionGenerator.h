#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "Instruction.h"
#include "SystemConfig.h"

class InstructionGenerator {
public:
    static std::vector<std::shared_ptr<Instruction>> generateInstructions(int pid, const SystemConfig& config);

private:
    static std::shared_ptr<Instruction> randomInstruction(int pid, std::unordered_set<std::string>& declaredVars, const SystemConfig& config, int layer = 0);

    static std::string randomVarName();
    static uint16_t randomUint16();
    static uint8_t randomSleepDuration();
    static std::string randomPrintMessage(int pid);
};