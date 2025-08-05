#include "ConsoleUtil.h"

// Splits the input string into tokens separated by whitespace
std::vector<std::string> ConsoleUtil::tokenizeInput(const std::string input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (stream >> token) { // Extract tokens separated by whitespace
        tokens.push_back(token);
    }

    return tokens;
}

// Finds a process by its name from the global scheduler
std::shared_ptr<Process> ConsoleUtil::findProcessByName (const std::string& name) {
    const auto all = GlobalScheduler::getInstance()->getAllProcesses();
    for (const auto& p : all) {
        if (p && p->getName() == name) { // Check if process name matches
            return p;
        }
    }
    return nullptr; // Not found
}

// Generates a timestamp string in the format "(MM/DD/YYYY HH:MM:SSAM/PM)"
std::string ConsoleUtil::generateTimestamp() {
    std::ostringstream oss;

    std::time_t t = std::time(nullptr); // Get current time

    std::tm now;

    localtime_s(&now, &t); // Convert to local time

    oss << std::put_time(&now, "(%m/%d/%Y %I:%M:%S%p)"); // Format timestamp

    return oss.str();
}

// Truncates long names to a maximum display length, prepending "..." if truncated
std::string ConsoleUtil::truncateLongNames(std::string name) {
    if (name.size() > 9) {
        const size_t maxDisplay = 9;
        const size_t visible = maxDisplay - 3;
        name = "..." + name.substr(name.size() - visible); // Show last characters
    }

    return name;
}

// Converts a uint32_t value to a hexadecimal string (uppercase)
std::string ConsoleUtil::toHex(uint32_t value) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << value;
    return ss.str();
}

// Logs an error message to std::cerr with "[ERROR]" prefix
void ConsoleUtil::logError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

// Returns a random power-of-2 value between min and max (inclusive)
uint32_t ConsoleUtil::rollBetween(uint32_t min, uint32_t max, uint32_t frameSize) {
    std::vector<uint32_t> valid;

    for (uint32_t exp = 0; exp <= 31; ++exp) {
        uint32_t val = 1u << exp;
        if (val >= min && val <= max) {
            valid.push_back(val); // Collect valid powers of 2
        }
    }

    if (valid.empty()) {
        throw std::runtime_error("No valid power-of-2 value between bounds divisible by frameSize.");
    }

    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> dist(0, valid.size() - 1);
    return valid[dist(rng)]; // Return a random valid value
}

// Trims whitespace from both ends of the input string
std::string ConsoleUtil::trim(const std::string& str) {
    const char* ws = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(ws);
    size_t end = str.find_last_not_of(ws);
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Unescapes \" and \\ in the input string
std::string ConsoleUtil::unescapeString(const std::string& input) {
    std::string output;
    output.reserve(input.size());

    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '\\' && i + 1 < input.length()) {
            if (input[i + 1] == '"') {
                output += '"'; // Unescape quote
                ++i;
            } else if (input[i + 1] == '\\') {
                output += '\\'; // Unescape backslash
                ++i;
            } else {
                output += input[i]; // Leave as is
            }
        } else {
            output += input[i];
        }
    }
    return output;
}