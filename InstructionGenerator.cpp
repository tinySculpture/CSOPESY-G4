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

// Maximum nested FOR depth to prevent runaway recursion
constexpr int MAX_FOR_DEPTH = 2;

std::vector<std::shared_ptr<Instruction>> InstructionGenerator::generateInstructions(
    int pid,
    const SystemConfig& config
) {
	// Determine how many instructions to generate based on the configuration
    int count = std::uniform_int_distribution<>(
        config.minInstructions, 
        config.maxInstructions
    )(rng);


    std::vector<std::shared_ptr<Instruction>> result;
    std::unordered_set<std::string> declaredVars;

    // Populate the instruction list
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
	// Define the possible instruction types
    enum { PRINT = 0, DECLARE, ADD, SUB, SLEEP, FOR };

	// Randomly select an instruction type
    int choice = std::uniform_int_distribution<>(0, 5)(rng);

    switch (choice) {
    case PRINT: {
        // Three subtypes: default hello, literal message, or variable value
        int subtype = std::uniform_int_distribution<>(0, 2)(rng);
        if (subtype == 0)
            return std::make_shared<PrintInstruction>(PrintType::Hello);
        else if (subtype == 1)
            return std::make_shared<PrintInstruction>(PrintType::Literal, randomPrintMessage(pid));
        else {
			// Ensure a variable exists by creating one for variable print
            std::string var = randomVarName();
            declaredVars.insert(var);
            return std::make_shared<PrintInstruction>(PrintType::Variable, var);
        }
    }

    case DECLARE: {
		// Declare a new variable with a random name and value
        std::string var = randomVarName();
        declaredVars.insert(var);
        return std::make_shared<DeclareInstruction>(var, randomUint16());
    }

    case ADD: {
		// Setup a new variable for the result of the addition
        std::string target = randomVarName();
        declaredVars.insert(target);

        // Helper to choose between existing variable or literal operand
        auto makeOperand = [&]() -> std::variant<std::string, uint16_t> {
            if (!declaredVars.empty() && std::uniform_int_distribution<>(0, 1)(rng) == 0) {
				// Pick a random variable from declaredVars
                auto it = declaredVars.begin();
                int index = static_cast<int>(declaredVars.size()) - 1;
                std::advance(it, std::uniform_int_distribution<>(0, index)(rng));
                return *it;
            }
            else {
				// Otherwise, generate a random literal value
                return randomUint16();
            }
        };

        auto op1 = makeOperand();
        auto op2 = makeOperand();

        return std::make_shared<AddInstruction>(target, op1, op2);
    }

    case SUB: {
		// Subtraction always uses two literals to avoid negative results
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
        // Sleep instruction with small random delay
        return std::make_shared<SleepInstruction>(randomSleepDuration());
    }

    case FOR: {
		// Build a nested FOR instruction if within depth limits
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
	// Fallback: minimal sleep if no other instruction was selected
    return std::make_shared<SleepInstruction>(1);
}

std::string InstructionGenerator::randomVarName() {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    std::string var = "v";
    var += charset[std::uniform_int_distribution<>(0, 25)(rng)];
    return var;
}

uint16_t InstructionGenerator::randomUint16() {
	// Generate a random unsigned 16-bit integer in the range [0, 500]
    return std::uniform_int_distribution<uint16_t>(0, 500)(rng);
}

uint8_t InstructionGenerator::randomSleepDuration() {
	// Generate a random sleep duration between 1 and 5 ticks
    return static_cast<uint8_t>(std::uniform_int_distribution<unsigned int>(1, 5)(rng));
}

std::string InstructionGenerator::randomPrintMessage(int pid) {
	// Format a unique message including the process ID
    std::stringstream ss;
    ss << "PID " << pid << " sent you a msg! :D";
    return ss.str();
}