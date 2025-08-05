#pragma once

#include <cstdint>
#include <string>

/**
 * @class SystemConfig
 * @brief Holds scheduler and system-wide parameters and provides
 *        loading, validation, and reporting functionality.
 *
 * Configuration options include:
 * - numCPU: Number of CPU cores (1-128)
 * - scheduler: Scheduling algorithm ("fcfs" or "rr")
 * - quantumCycles: Time slice for round-robin scheduling
 * - batchProcessFreq: Frequency of auto-generated processes
 * - minInstructions/maxInstructions: Range for per-process instruction count
 * - delaysPerExec: Busy-wait ticks between instruction executions
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

    unsigned long maxOverallMemory = 16384;
	unsigned long memoryPerFrame = 16;
	unsigned long minMemoryPerProcess = 1024;
	unsigned long maxMemoryPerProcess = 1024;

    static void initialize(const std::string& filename);

    static SystemConfig* getInstance();

    static void destroy();

    /**
     * @brief Validates and clamps members to acceptable ranges,
     *        printing warnings and resetting to defaults if invalid.
     */
    void validate() const;



    /**
     * @brief Print the current configuration to stdout.
     */
    void printSystemConfig() const;

private:
    SystemConfig(const std::string& filename);

    inline static SystemConfig* sharedInstance = nullptr; // Singleton instance

    /**
     * @brief Check if a path refers to an existing regular file.
     *
     * @param path File path to check.
     * @return true if exists and is a regular file.
     */
    static bool fileExists(const std::string& path);



    /**
     * @brief Determine if a line is blank or a comment.
     *
     * Lines consisting only of whitespace or starting with '#' are
     * considered non-meaningful and are skipped.
     *
     * @param line Input text line.
     * @return true if blank or comment.
     */
    static bool isWhitespaceOrComment(const std::string& line);
};