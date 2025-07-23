#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "SystemConfig.h"
#include "ColorUtil.h"

using Color = ColorUtil::Color;
namespace CU = ColorUtil;

void SystemConfig::validate() const {
    if (numCPU < 1 || numCPU > 128) {
        CU::printColoredText(Color::Yellow, "[!] num-cpu must be in the range [1, 128]. Using default value of 4.\n");
        const_cast<SystemConfig*>(this)->numCPU = 4;
    }

    if (scheduler != "rr" && scheduler != "fcfs") {
        CU::printColoredText(Color::Yellow, "[!] invalid scheduler. Must be 'fcfs' (first-come-first-serve) or 'rr' (round-robin). Using default value of 'rr'.\n");
        const_cast<SystemConfig*>(this)->scheduler = "rr";
    }

    if (quantumCycles < 1 || quantumCycles > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] quantum-cycles must be in the range [1, 4294967295]. Using default value of 5.\n");
        const_cast<SystemConfig*>(this)->quantumCycles = 5;
    }

    if (batchProcessFreq < 1 || batchProcessFreq > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] batch-process-freq must be in the range [1, 4294967295]. Using default value of 1.\n");
        const_cast<SystemConfig*>(this)->batchProcessFreq = 1;
    }

    if (minInstructions < 1 || minInstructions > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] min-ins must be in the range [1, 4294967295]. Using default value of 1000.\n");
        const_cast<SystemConfig*>(this)->minInstructions = 1000;
    }

    if (maxInstructions < 1 || maxInstructions > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] max-ins must be in the range [1, 4294967295]. Using default value of 2000.\n");
        const_cast<SystemConfig*>(this)->maxInstructions = 2000;
    }

    if (minInstructions > maxInstructions) {
        CU::printColoredText(Color::Yellow, "[!] min-ins cannot be greater than max-ins. Using default values of 1000 and 2000 respectively.\n");
        const_cast<SystemConfig*>(this)->minInstructions = 1000;
        const_cast<SystemConfig*>(this)->maxInstructions = 2000;
    }

    if (delaysPerExec < 0 || delaysPerExec > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] delays-per-exec must be in the range [0, 4294967295]. Using default value of 0.\n");
        const_cast<SystemConfig*>(this)->delaysPerExec = 0;
    }

    if (maxOverallMemory < 1 || maxOverallMemory > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] max-overall-mem must be in the range [1, 4294967295]. Using default value of 16384.\n");
        const_cast<SystemConfig*>(this)->maxOverallMemory = 16384;
	}

    if (memoryPerFrame < 1 || memoryPerFrame > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] mem-per-frame must be in the range [1, 4294967295]. Using default value of 16.\n");
        const_cast<SystemConfig*>(this)->memoryPerFrame = 16;
    }

    if (memoryPerProcess < 1 || memoryPerProcess > 4294967295) {
        CU::printColoredText(Color::Yellow, "[!] mem-per-proc must be in the range [1, 4294967295]. Using default value of 1024.\n");
        const_cast<SystemConfig*>(this)->memoryPerProcess = 1024;
	}
}
SystemConfig SystemConfig::loadFromFile(const std::string& filename) {
    SystemConfig config;
    std::ifstream file(filename);

    if (!fileExists(filename)) {
        CU::printColoredText(CU::Color::Yellow, "[!] Config file \"" + filename + "\" not found. Using defaults.\n");
        return config;
    }

    if (!file.is_open()) {
        CU::printColoredText(CU::Color::Red, "[X] Failed to open config file \"" + filename + "\". Using defaults.\n");
        return config;
    }

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
            if (key == "num-cpu") config.numCPU = std::stoi(value);
            else if (key == "scheduler") {
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                config.scheduler = value;
            }
            else if (key == "quantum-cycles") config.quantumCycles = std::stol(value);
            else if (key == "batch-process-freq") config.batchProcessFreq = std::stol(value);
            else if (key == "min-ins") config.minInstructions = std::stol(value);
            else if (key == "max-ins") config.maxInstructions = std::stol(value);
            else if (key == "delays-per-exec") config.delaysPerExec = std::stol(value);
            else if (key == "max-overall-mem") config.maxOverallMemory = std::stol(value);
            else if (key == "mem-per-frame") config.memoryPerFrame = std::stol(value);
			else if (key == "mem-per-proc") config.memoryPerProcess = std::stol(value);
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

    config.validate();
    return config;
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
    std::cout << "Memory per Process  : " << memoryPerProcess << "\n";
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