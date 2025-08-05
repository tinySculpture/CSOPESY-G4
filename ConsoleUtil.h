#pragma once

#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include "GlobalScheduler.h"
#include "ConsoleUtil.h"
#include "Process.h"

/**
 * @namespace ConsoleUtil
 * @brief Utility functions for console operations and process management.
 *
 * This namespace provides a collection of helper functions for tokenizing input,
 * finding processes, generating timestamps, formatting strings, logging errors,
 * generating random numbers, and string manipulation.
 */

/**
 * @brief Splits the input string into tokens separated by whitespace.
 * @param input The input string to tokenize.
 * @return A vector of tokens as strings.
 */
std::vector<std::string> tokenizeInput(const std::string input);

/**
 * @brief Finds a process by its name.
 * @param name The name of the process to search for.
 * @return A shared pointer to the found Process, or nullptr if not found.
 */
std::shared_ptr<Process> findProcessByName(const std::string& name);

/**
 * @brief Generates a timestamp string representing the current date and time.
 * @return A string containing the formatted timestamp.
 */
std::string generateTimestamp();

/**
 * @brief Truncates a long process or file name to a shorter version for display.
 * @param name The original name string.
 * @return The truncated name string.
 */
std::string truncateLongNames(std::string name);

/**
 * @brief Converts a 32-bit unsigned integer to its hexadecimal string representation.
 * @param value The value to convert.
 * @return The hexadecimal string representation of the value.
 */
std::string toHex(uint32_t value);

/**
 * @brief Logs an error message to the console or log file.
 * @param message The error message to log.
 */
void logError(const std::string& message);

/**
 * @brief Generates a random number between min and max, possibly using a frame size for seeding.
 * @param min The minimum value (inclusive).
 * @param max The maximum value (inclusive).
 * @param frameSize The frame size used for randomization.
 * @return A random number between min and max.
 */
uint32_t rollBetween(uint32_t min, uint32_t max, uint32_t frameSize);

/**
 * @brief Trims leading and trailing whitespace from a string.
 * @param str The string to trim.
 * @return The trimmed string.
 */
std::string trim(const std::string& str);

/**
 * @brief Unescapes escape sequences in a string (e.g., converts "\\n" to newline).
 * @param input The string containing escape sequences.
 * @return The unescaped string.
 */
std::string unescapeString(const std::string& input);
namespace ConsoleUtil
{
	std::vector<std::string> tokenizeInput(const std::string input);
	std::shared_ptr<Process> findProcessByName(const std::string& name);
	std::string generateTimestamp();
	std::string truncateLongNames(std::string name);
	std::string toHex(uint32_t value);
	void logError(const std::string& message);
	uint32_t rollBetween(uint32_t min, uint32_t max, uint32_t frameSize);
	std::string trim(const std::string& str);
	std::string unescapeString(const std::string& input);

}