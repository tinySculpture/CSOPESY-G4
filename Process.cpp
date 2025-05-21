#include "Process.h"

#include <ctime>
#include <iomanip>
#include <sstream>

Process::Process(const std::string& name, int currentInstruction, int totalInstructions): 
    name(name), currentInstruction(currentInstruction), totalInstructions(totalInstructions) {
    timestamp = generateTimestamp();
}

std::string Process::getName() const {
    return name;
}

int Process::getCurrentInstruction() const {
    return currentInstruction;
}

int Process::getTotalInstructions() const {
    return totalInstructions;
}

std::string Process::getTimestamp() const {
    return timestamp;
}

std::string Process::generateTimestamp() const {
    std::ostringstream oss;
    std::time_t t = std::time(nullptr);
    std::tm now = *std::localtime(&t);
    oss << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}