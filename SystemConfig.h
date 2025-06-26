#pragma once

#include <cstdint>
#include <string>

class SystemConfig {
public:
    int numCPU = 4;
    std::string scheduler = "rr";
    unsigned long quantumCycles = 5;
    unsigned long batchProcessFreq = 1;
    unsigned long minInstructions = 1000;
    unsigned long maxInstructions = 2000;
    unsigned long delaysPerExec = 0;

    void validate() const;
    static SystemConfig loadFromFile(const std::string& filename);
    void printSystemConfig() const;

private:
    static bool fileExists(const std::string& path);
    static bool isWhitespaceOrComment(const std::string& line);
};