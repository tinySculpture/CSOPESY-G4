#include <random>
#include <sstream>
#include <unordered_set>
#include <variant>

#include "InstructionGenerator.h"
#include "AddInstruction.h"
#include "DeclareInstruction.h"
#include "ForInstruction.h"
#include "PrintInstruction.h"
#include "SleepInstruction.h"
#include "SubtractInstruction.h"

static std::mt19937 rng(std::random_device{}());
constexpr int MAX_FOR_DEPTH = 2;

std::vector<std::shared_ptr<Instruction>> InstructionGenerator::generateInstructions(
    int pid,
    const SystemConfig& config
) {
    int count = std::uniform_int_distribution<>(config.minInstructions, config.maxInstructions)(rng);
    std::vector<std::shared_ptr<Instruction>> result;
    std::unordered_set<std::string> declaredVars;

    for (int i = 0; i < count; ++i) {
        result.push_back(randomInstruction(pid, declaredVars, config, 0));
    }

    return result;
}

std::shared_ptr<Instruction> InstructionGenerator::randomInstruction(
    int pid,
    std::unordered_set<std::string>& declaredVars,
    const SystemConfig& config, 
    int layer
) {
    enum { PRINT = 0, DECLARE, ADD, SUB, SLEEP, FOR };

    int choice = std::uniform_int_distribution<>(0, 5)(rng);

    switch (choice) {
    case PRINT: {
        int subtype = std::uniform_int_distribution<>(0, 2)(rng);
        if (subtype == 0)
            return std::make_shared<PrintInstruction>(PrintType::Hello);
        else if (subtype == 1)
            return std::make_shared<PrintInstruction>(PrintType::Literal, randomPrintMessage(pid));
        else {
            std::string var = randomVarName();
            declaredVars.insert(var);
            return std::make_shared<PrintInstruction>(PrintType::Variable, var);
        }
    }

    case DECLARE: {
        std::string var = randomVarName();
        declaredVars.insert(var);
        return std::make_shared<DeclareInstruction>(var, randomUint16());
    }

    case ADD: {
        std::string target = randomVarName();
        declaredVars.insert(target);

        auto makeOperand = [&]() -> std::variant<std::string, uint16_t> {
            if (!declaredVars.empty() && std::uniform_int_distribution<>(0, 1)(rng) == 0) {
                auto it = declaredVars.begin();
                int index = static_cast<int>(declaredVars.size()) - 1;
                std::advance(it, std::uniform_int_distribution<>(0, index)(rng));
                return *it;
            }
            else {
                return randomUint16();
            }
            };

        auto op1 = makeOperand();
        auto op2 = makeOperand();

        return std::make_shared<AddInstruction>(target, op1, op2);
    }

    case SUB: {
        std::string target = randomVarName();
        declaredVars.insert(target);

        uint16_t val1 = randomUint16();
        uint16_t val2 = randomUint16();
        if (val1 < val2) std::swap(val1, val2);

        auto op1 = val1;
        auto op2 = val2;

        return std::make_shared<SubtractInstruction>(target, op1, op2);
    }

    case SLEEP: {
        return std::make_shared<SleepInstruction>(randomSleepDuration());
    }

    case FOR: {
        if (layer > MAX_FOR_DEPTH) break;

        int loopCount = std::uniform_int_distribution<>(1, 4)(rng);
        auto forInstr = std::make_shared<ForInstruction>(loopCount, layer);

        int innerCount = std::uniform_int_distribution<>(2, 4)(rng);
        for (int i = 0; i < innerCount; ++i) {
            auto innerInstr = randomInstruction(pid, declaredVars, config, layer + 1);
            if (innerInstr) {
                forInstr->addInstruction(innerInstr);
            }
        }

        return forInstr;
    }
    }

    return std::make_shared<SleepInstruction>(1);
}

std::string InstructionGenerator::randomVarName() {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    std::string var = "v";
    var += charset[std::uniform_int_distribution<>(0, 25)(rng)];
    return var;
}

uint16_t InstructionGenerator::randomUint16() {
    return std::uniform_int_distribution<uint16_t>(0, 500)(rng);
}

uint8_t InstructionGenerator::randomSleepDuration() {
    return static_cast<uint8_t>(std::uniform_int_distribution<unsigned int>(1, 5)(rng));
}

std::string InstructionGenerator::randomPrintMessage(int pid) {
    std::stringstream ss;
    ss << "PID " << pid << " sent you a msg! :D";
    return ss.str();
}