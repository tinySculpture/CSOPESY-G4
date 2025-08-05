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
#include "ReadInstruction.h"
#include "WriteInstruction.h"

// Static random number generator for the class
static std::mt19937 rng(std::random_device{}());

// Maximum allowed nesting depth for FOR instructions
constexpr int MAX_FOR_DEPTH = 2;

/**
 * @brief Generates a vector of random instructions for a process.
 * 
 * @param pid Process ID
 * @param config System configuration (min/max instructions, etc.)
 * @param allocatedMemory Amount of memory allocated to the process
 * @return std::vector<std::shared_ptr<Instruction>> Generated instructions
 */
std::vector<std::shared_ptr<Instruction>> InstructionGenerator::generateInstructions(
    int pid,
    const SystemConfig& config,
    uint16_t allocatedMemory
) {
    // Randomly determine the number of instructions to generate
    int count = std::uniform_int_distribution<>(
        config.minInstructions, 
        config.maxInstructions
    )(rng);

    std::vector<std::shared_ptr<Instruction>> result;
    std::unordered_set<std::string> declaredVars; // Track declared variable names
    std::unordered_map<std::string, uint16_t> addressVars; // Track address variables

    int i = 0;
    while (i < count) {
        // Generate a random instruction
        auto instr = randomInstruction(pid, declaredVars, addressVars, config, allocatedMemory, 0);
        // If the instruction is a FOR loop, flatten its nested instructions
        if (auto forInstr = std::dynamic_pointer_cast<ForInstruction>(instr)) {
            auto nestedInstructions = forInstr->flattenInstructions();

            // Limit nested instructions to fit within the total count
            if (nestedInstructions.size() > count - result.size()) {
                nestedInstructions.resize(count - result.size());
            }

            for (const auto& nestedInstr : nestedInstructions) {
                if (nestedInstr) {
                    result.push_back(nestedInstr);
                    ++i; 
                }
            }
        }
        else {
            result.push_back(instr);
            ++i; 
        }
    }

    return result;
}

/**
 * @brief Generates a single random instruction, possibly recursive for FOR loops.
 * 
 * @param pid Process ID
 * @param declaredVars Set of declared variable names (in/out)
 * @param addressVars Map of address variable names to addresses (in/out)
 * @param config System configuration
 * @param allocatedMemory Amount of memory allocated
 * @param layer Current FOR loop nesting depth
 * @return std::shared_ptr<Instruction> Generated instruction
 */
std::shared_ptr<Instruction> InstructionGenerator::randomInstruction(
    int pid,
    std::unordered_set<std::string>& declaredVars,
    std::unordered_map<std::string, uint16_t>& addressVars,
    const SystemConfig& config,
    uint16_t allocatedMemory,
    int layer
) {
    enum { PRINT = 0, DECLARE, ADD, SUB, SLEEP, FOR, READ, WRITE };

    int choice = std::uniform_int_distribution<>(0, 7)(rng);

    switch (choice) {
    case PRINT: {
        // Randomly choose print subtype
        int subtype = std::uniform_int_distribution<>(0, 2)(rng);
        if (subtype == 0)
            return std::make_shared<PrintInstruction>(PrintType::Hello);
        else if (subtype == 1)
            return std::make_shared<PrintInstruction>(PrintType::Literal, randomPrintMessage(pid));
        else {
            // Print variable value if any declared
            if (declaredVars.empty()) break;
            auto it = declaredVars.begin();
            std::advance(it, std::uniform_int_distribution<>(0, declaredVars.size() - 1)(rng));
            return std::make_shared<PrintInstruction>(PrintType::Variable, *it);
        }
    }

    case DECLARE: {
        // Declare a new variable, possibly as an address
        std::string var = randomVarName();
        declaredVars.insert(var);

        bool isAddress = std::uniform_int_distribution<>(0, 1)(rng) == 0;
        uint16_t value;

        if (isAddress) {
            // Address variable: pick a valid address
            if (allocatedMemory < 2) return std::make_shared<PrintInstruction>(PrintType::Hello);
            value = std::uniform_int_distribution<uint16_t>(0, allocatedMemory - 2)(rng);
            addressVars[var] = value;
        } else {
            // Regular variable: random value
            value = randomUint16();
        }

        return std::make_shared<DeclareInstruction>(var, value);
    }

    case ADD: {
        // Add two operands and store in a new variable
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
        // Subtract two random values and store in a new variable
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
        // Sleep for a random duration
        return std::make_shared<SleepInstruction>(randomSleepDuration());
    }

    case FOR: {
        // Generate a FOR loop with nested instructions, up to max depth
        if (layer > MAX_FOR_DEPTH) break;

        int loopCount = std::uniform_int_distribution<>(1, 4)(rng);
        auto forInstr = std::make_shared<ForInstruction>(loopCount, layer);

        int innerCount = std::uniform_int_distribution<>(2, 4)(rng);
        for (int i = 0; i < innerCount; ++i) {
            auto innerInstr = randomInstruction(pid, declaredVars, addressVars, config, layer + 1);
            if (innerInstr) {
                forInstr->addInstruction(innerInstr);
            }
        }

        return forInstr;
    }

    case READ: {
        // Read from an address variable into a new variable
        if (addressVars.empty()) break;

        std::string targetVar = randomVarName();
        declaredVars.insert(targetVar);

        auto it = addressVars.begin();
        std::advance(it, std::uniform_int_distribution<>(0, addressVars.size() - 1)(rng));
        std::string addrVar = it->first;

        return std::make_shared<ReadInstruction>(targetVar, addrVar);
    }

    case WRITE: {
        // Write a variable's value to an address variable
        if (addressVars.empty() || declaredVars.empty()) break;

        auto it = addressVars.begin();
        std::advance(it, std::uniform_int_distribution<>(0, addressVars.size() - 1)(rng));
        std::string addrVar = it->first;

        auto it2 = declaredVars.begin();
        std::advance(it2, std::uniform_int_distribution<>(0, declaredVars.size() - 1)(rng));
        std::string valVar = *it2;

        return std::make_shared<WriteInstruction>(addrVar, valVar);
    }
    }
    // Fallback: print hello if no valid instruction was generated
    return std::make_shared<PrintInstruction>(PrintType::Hello);
}

/**
 * @brief Generates a random variable name.
 * 
 * @return std::string Random variable name (e.g., "varA")
 */
std::string InstructionGenerator::randomVarName() {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string var = "var";
    var += charset[std::uniform_int_distribution<>(0, 25)(rng)];
    return var;
}

/**
 * @brief Generates a random 16-bit unsigned integer.
 * 
 * @return uint16_t Random value in [0, 500]
 */
uint16_t InstructionGenerator::randomUint16() {
    return std::uniform_int_distribution<uint16_t>(0, 500)(rng);
}

/**
 * @brief Generates a random sleep duration.
 * 
 * @return uint8_t Random duration in [1, 5]
 */
uint8_t InstructionGenerator::randomSleepDuration() {
    return static_cast<uint8_t>(std::uniform_int_distribution<unsigned int>(1, 5)(rng));
}

/**
 * @brief Generates a random print message for a given process.
 * 
 * @param pid Process ID
 * @return std::string Message string
 */
std::string InstructionGenerator::randomPrintMessage(int pid) {
    std::stringstream ss;
    ss << "PID " << pid << " sent you a msg! :D";
    return ss.str();
}

/**
 * @brief Parses a vector of raw instruction strings into Instruction objects.
 * 
 * @param rawInstructions Vector of instruction strings
 * @return std::vector<std::shared_ptr<Instruction>> Parsed instructions
 * @throws std::runtime_error If any instruction string is invalid
 */
std::vector<std::shared_ptr<Instruction>> InstructionGenerator::parseCustomInstructions(
    const std::vector<std::string>& rawInstructions
) {
    std::vector<std::shared_ptr<Instruction>> parsed;

    for (const std::string& line : rawInstructions) {
        std::shared_ptr<Instruction> instr = Instruction::fromString(line);
        if (!instr) {
            throw std::runtime_error("Invalid instruction: " + line);
        }
        parsed.push_back(instr);
    }

    return parsed;
}
