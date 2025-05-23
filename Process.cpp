#include "Process.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>

/**
 * @brief Constructs a new Process object with a given name.
 *        The total number of instructions is randomly generated between 5 and 15.
 *        The current instruction is randomly set to a value between 1 and totalInstructions.
 *        A timestamp is also generated at the moment of creation.
 *
 * @param name The name/identifier for the process.
 */
Process::Process(const std::string& name) : name(name) {
    // Random number generator setup
    std::random_device rd;                         // Seed source for randomness
    std::mt19937 gen(rd());                        // Mersenne Twister engine for pseudo-random generation

    // Random total instructions between 5 and 15
    std::uniform_int_distribution<> distTotal(5, 15);
    totalInstructions = distTotal(gen);

    // Random current instruction between 1 and totalInstructions
    std::uniform_int_distribution<> distCurrent(1, totalInstructions);
    currentInstruction = distCurrent(gen);

    // Generate a formatted timestamp string
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
 * @brief Gets the current instruction number the process is executing.
 *
 * @return int Current instruction index (1-based).
 */
int Process::getCurrentInstruction() const {
    return currentInstruction;
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
