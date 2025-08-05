#pragma once

#include <cstdint>
#include <string>

/**
 * @class SystemConfig
 * @brief Represents the configuration settings for the operating system simulation.
 *
 * This class encapsulates various system parameters such as CPU count, scheduler type,
 * quantum cycles, memory limits, and process instruction limits. It provides methods
 * for validating the configuration, loading settings from a file, and printing the current
 * configuration.
 *
 * @var int numCPU
 *      Number of CPUs available in the system.
 * @var std::string scheduler
 *      The scheduling algorithm to use (e.g., "rr" for round-robin).
 * @var unsigned long quantumCycles
 *      Number of cycles per quantum for the scheduler.
 * @var unsigned long batchProcessFreq
 *      Frequency at which batch processes are scheduled.
 * @var unsigned long minInstructions
 *      Minimum number of instructions per process.
 * @var unsigned long maxInstructions
 *      Maximum number of instructions per process.
 * @var unsigned long delaysPerExec
 *      Number of delays per execution cycle.
 * @var unsigned long maxOverallMemory
 *      Maximum overall memory available in the system (in bytes).
 * @var unsigned long memoryPerFrame
 *      Memory size per frame (in bytes).
 * @var unsigned long minMemoryPerProcess
 *      Minimum memory allocated per process (in bytes).
 * @var unsigned long maxMemoryPerProcess
 *      Maximum memory allocated per process (in bytes).
 *
 * @fn void validate() const
 *      Validates the current configuration parameters.
 * @fn static SystemConfig loadFromFile(const std::string& filename)
 *      Loads configuration settings from a specified file.
 * @fn void printSystemConfig() const
 *      Prints the current system configuration to the standard output.
 *
 * @private
 * @fn static bool fileExists(const std::string& path)
 *      Checks if the specified file exists.
 * @fn static bool isWhitespaceOrComment(const std::string& line)
 *      Determines if a line is whitespace or a comment.
 */
class SystemConfig {
public:
    int numCPU = 4;
    std::string scheduler = "rr";
    unsigned long quantumCycles = 5;
    unsigned long batchProcessFreq = 1;
    unsigned long minInstructions = 1000;
    unsigned long maxInstructions = 2000;
    unsigned long delaysPerExec = 0;

    unsigned long maxOverallMemory = 4096;
	unsigned long memoryPerFrame = 256;
	unsigned long minMemoryPerProcess = 512;
    unsigned long maxMemoryPerProcess = 1024;

    void validate() const;
    static SystemConfig loadFromFile(const std::string& filename);
    void printSystemConfig() const;

private:
    static bool fileExists(const std::string& path);
    static bool isWhitespaceOrComment(const std::string& line);
};