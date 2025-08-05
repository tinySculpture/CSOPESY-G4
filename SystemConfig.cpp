#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "SystemConfig.h"
#include "ColorUtil.h"

using Color = ColorUtil::Color;
namespace CU = ColorUtil;

SystemConfig::SystemConfig(const std::string& filename) {}

void SystemConfig::initialize(const std::string& filename) {
    if (sharedInstance == nullptr) {
        sharedInstance = new SystemConfig(filename);

        std::ifstream file(filename);

        if (!fileExists(filename)) {
            CU::printColoredText(CU::Color::Yellow, "[!] Config file \"" + filename + "\" not found. Using defaults.\n");
        } else if (!file.is_open()) {
            CU::printColoredText(CU::Color::Red, "[X] Failed to open config file \"" + filename + "\". Using defaults.\n");
        } else {
            std::string line;
            size_t meaningfulLines = 0;

            while (std::getline(file, line)) {
                if (isWhitespaceOrComment(line))
                    continue;

                std::istringstream iss(line);
                std::string key, value;
                if (!(iss >> key >> value)) {
                    CU::printColoredText(CU::Color::Red, "[X] Malformed config line: \"" + line + "\"\n");
                    continue;
                }
                meaningfulLines++;

                try {
                    if (key == "num-cpu") sharedInstance->numCPU = std::stoi(value);
                    else if (key == "scheduler") {
                        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                            value = value.substr(1, value.size() - 2);
                        }
                        sharedInstance->scheduler = value;
                    }
                    else if (key == "quantum-cycles") sharedInstance->quantumCycles = std::stol(value);
                    else if (key == "batch-process-freq") sharedInstance->batchProcessFreq = std::stol(value);
                    else if (key == "min-ins") sharedInstance->minInstructions = std::stol(value);
                    else if (key == "max-ins") sharedInstance->maxInstructions = std::stol(value);
                    else if (key == "delays-per-exec") sharedInstance->delaysPerExec = std::stol(value);
                    else if (key == "max-overall-mem") sharedInstance->maxOverallMemory = std::stol(value);
                    else if (key == "mem-per-frame") sharedInstance->memoryPerFrame = std::stol(value);
                    else if (key == "min-mem-per-proc") sharedInstance->minMemoryPerProcess = std::stol(value);
                    else if (key == "max-mem-per-proc") sharedInstance->maxMemoryPerProcess = std::stol(value);
                    else { CU::printColoredText(CU::Color::Red, "[X] Unknown config key: \"" + key + "\"\n"); }
                }
                catch (...) {
                    continue;
                }
            }

            if (meaningfulLines == 0) {
                CU::printColoredText(CU::Color::Yellow, "[!] Configuration file is empty. Using defaults.\n");
            }
            else if (meaningfulLines == 7) {
                CU::printColoredText(CU::Color::Green, "[*] Configuration successfully loaded from \"" + filename + "\".\n");
            }
        }

        sharedInstance->validate();
    }
}

void SystemConfig::destroy() {
    delete sharedInstance;
    sharedInstance = nullptr;
}

void SystemConfig::validate() const {
    if (numCPU < 1 || numCPU > 128) {
        CU::printColoredText(Color::Yellow, "[!] num-cpu must be in the range [1, 128]. Using default value of 4.\n");
        sharedInstance->numCPU = 4;
    }

    if (scheduler != "rr" && scheduler != "fcfs") {
        CU::printColoredText(Color::Yellow, "[!] invalid scheduler. Must be 'fcfs' (first-come-first-serve) or 'rr' (round-robin). Using default value of 'rr'.\n");
        sharedInstance->scheduler = "rr";
    }

    if (quantumCycles < 1 || quantumCycles > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] quantum-cycles must be in the range [1, 4294967295]. Using default value of 5.\n");
        sharedInstance->quantumCycles = 5;
    }

    if (batchProcessFreq < 1 || batchProcessFreq > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] batch-process-freq must be in the range [1, 4294967295]. Using default value of 1.\n");
        sharedInstance->batchProcessFreq = 1;
    }

    if (minInstructions < 1 || minInstructions > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] min-ins must be in the range [1, 4294967295]. Using default value of 1000.\n");
        sharedInstance->minInstructions = 1000;
    }

    if (maxInstructions < 1 || maxInstructions > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] max-ins must be in the range [1, 4294967295]. Using default value of 2000.\n");
        sharedInstance->maxInstructions = 2000;
    }

    if (minInstructions > maxInstructions) {
        CU::printColoredText(Color::Yellow, "[!] min-ins cannot be greater than max-ins. Using default values of 1000 and 2000 respectively.\n");
        sharedInstance->minInstructions = 1000;
        sharedInstance->maxInstructions = 2000;
    }

    if (delaysPerExec < 0 || delaysPerExec > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] delays-per-exec must be in the range [0, 4294967295]. Using default value of 0.\n");
        sharedInstance->delaysPerExec = 0;
    }

    // NEW CONFIGS FOR MO2

    auto isPowerOfTwo = [](uint64_t x) {
        return x && ((x & (x - 1)) == 0);
    };

    constexpr uint64_t MIN_MEM = 1ULL << 6;   // 64
    constexpr uint64_t MAX_MEM = 1ULL << 16;  // 65536

    if (!isPowerOfTwo(maxOverallMemory) || maxOverallMemory < MIN_MEM || maxOverallMemory > MAX_MEM) {
        CU::printColoredText(Color::Yellow,
            "[!] max-overall-mem must be a power of two in the range [64, 65536]. Using default value of 16384.\n");
        sharedInstance->maxOverallMemory = 16384; // 2^14, valid
    }

    if (!isPowerOfTwo(memoryPerFrame) || memoryPerFrame < MIN_MEM || memoryPerFrame > MAX_MEM) {
        CU::printColoredText(Color::Yellow,
            "[!] mem-per-frame must be a power of two in the range [64, 65536]. Using default value of 64.\n");
        sharedInstance->memoryPerFrame = 64; // adjusted default to 2^6
    }

    if (!isPowerOfTwo(minMemoryPerProcess) || minMemoryPerProcess < MIN_MEM || minMemoryPerProcess > MAX_MEM) {
        CU::printColoredText(Color::Yellow,
            "[!] mem-per-proc (min) must be a power of two in the range [64, 65536]. Using default value of 1024.\n");
        sharedInstance->minMemoryPerProcess = 1024; // 2^10
    }

    if (!isPowerOfTwo(maxMemoryPerProcess) || maxMemoryPerProcess < MIN_MEM || maxMemoryPerProcess > MAX_MEM) {
        CU::printColoredText(Color::Yellow,
            "[!] mem-per-proc (max) must be a power of two in the range [64, 65536]. Using default value of 1024.\n");
        sharedInstance->maxMemoryPerProcess = 1024; // 2^10
    }
}


void SystemConfig::printSystemConfig() const {
    std::cout << "CPUs                : " << numCPU << "\n";
    std::cout << "Scheduler           : " << scheduler << "\n";
    std::cout << "Quantum Cycles      : " << quantumCycles << "\n";
    std::cout << "Batch Process Freq  : " << batchProcessFreq << "\n";
    std::cout << "Min Instructions    : " << minInstructions << "\n";
    std::cout << "Max Instructions    : " << maxInstructions << "\n";
    std::cout << "Delays per Exec     : " << delaysPerExec << "\n";
	std::cout << "Max Overall Memory  : " << maxOverallMemory << "\n";
	std::cout << "Memory per Frame    : " << memoryPerFrame << "\n";
    std::cout << "Minimum Memory per Process  : " << minMemoryPerProcess << "\n";
	std::cout << "Maximum Memory per Process  : " << maxMemoryPerProcess << "\n";
}

bool SystemConfig::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool SystemConfig::isWhitespaceOrComment(const std::string& line) {
    for (char c : line) {
        if (!std::isspace(c)) {
            return c == '#';
        }
    }
    return true;
}

SystemConfig* SystemConfig::getInstance() {
    return sharedInstance;
}