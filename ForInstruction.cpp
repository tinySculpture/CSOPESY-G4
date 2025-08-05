#include "ForInstruction.h"   

ForInstruction::ForInstruction(int loopCount, int layer)
    : loopCount(loopCount), layer(layer) {
}

void ForInstruction::addInstruction(const std::shared_ptr<Instruction>& instr) {
    if (instr) {
        innerInstructions.push_back(instr);
    }
}

int ForInstruction::execute(Process& process) {
    return 0;
}

std::vector<std::shared_ptr<Instruction>> ForInstruction::flattenInstructions() {
	std::vector<std::shared_ptr<Instruction>> flattenedInstructions;
    
    // Flatten the inner instructions and nested ForInstruction objects into a single vector
    for (const auto& instr : innerInstructions) {
        if (auto forInstr = std::dynamic_pointer_cast<ForInstruction>(instr)) {
            // Recursively flatten nested ForInstructions
            auto nestedInstructions = forInstr->flattenInstructions();
            flattenedInstructions.insert(flattenedInstructions.end(),
                                         nestedInstructions.begin(),
                                         nestedInstructions.end());
        } else {
            // Add regular instructions directly
            flattenedInstructions.push_back(instr);
        }
	}

    return flattenedInstructions;
}

std::string ForInstruction::toString() const {
    std::stringstream ss;
    ss << "FOR " << loopCount << " times";
    return ss.str();
}