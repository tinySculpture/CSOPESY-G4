#include "Process.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <random>

/**
 * @brief Constructs a new Process object with a given name.
 *        The total number of instructions is randomly generated between 5 and 15.
 *        The current instruction is randomly set to a value between 1 and totalInstructions.
 *        A timestamp is also generated at the moment of creation.
 *
 * @param name The name/identifier for the process.
 */
Process::Process(const std::string& name, int numInstructions) 
    : name(name), 
      totalInstructions(numInstructions), 
      remainingInstruction(numInstructions),
      coreID(-1) {
	timestamp = generateTimestamp();
}

/**
 * @brief Gets the name of the process.
 *
 * @return std::string The name of the process.
 */
std::string Process::getName() const {
    return name;
}

/**
 * @brief Gets the current instruction number of the process.
 *
 * @return int The current instruction number.
 */
int Process::getRemainingInstruction() const {
	return remainingInstruction; // Assuming this is the total instructions left
}

/**
 * @brief Gets the total number of instructions for the process.
 *
 * @return int Total number of instructions.
 */
int Process::getTotalInstructions() const {
    return totalInstructions;
}

/**
 * @brief Gets the timestamp of when the process was created.
 *
 * @return std::string Formatted timestamp string.
 */
std::string Process::getTimestamp() const {
    return timestamp;
}

/**
 * @brief Generates a human-readable timestamp string representing the current time.
 *
 * @return std::string Timestamp in the format "MM/DD/YYYY, HH:MM:SS AM/PM".
 */
std::string Process::generateTimestamp() const {
    std::ostringstream oss;
    std::time_t t = std::time(nullptr);     // Get current system time
    std::tm now;
    localtime_s(&now, &t);                  // Convert to local time safely
    oss << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p"); // Format as string
    return oss.str();
}

void Process::executeInstruction() {
    if (remainingInstruction > 0) {
		remainingInstruction--; // Decrement the remaining instruction count
    }
    else {
	    std::cout << "Process " << name << " has completed all instructions.\n" << std::endl;
    }
}

bool Process::isComplete() {
    return remainingInstruction == 0;
}

int Process::getCoreID() {
    return coreID;
}

int Process::setCoreID(int id) {
    coreID = id;
    return coreID;
}
