#include "Process.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>

// Default constructor with randomization | CHANGE THIS FOR THE FOLLOWING HWS
Process::Process(const std::string& name) : name(name) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distTotal(5, 15);
    totalInstructions = distTotal(gen);

    std::uniform_int_distribution<> distCurrent(1, totalInstructions);
    currentInstruction = distCurrent(gen);

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
    std::tm now;
    localtime_s(&now, &t);
    oss << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}