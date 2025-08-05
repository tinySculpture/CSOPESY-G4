#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

#include "Process.h"
#include "SystemConfig.h"
#include "MemoryManager.h"
#include "ConsoleUtil.h"
#include "Instruction.h"

class ReadInstruction : public Instruction {
public:
	ReadInstruction(const std::string& targetVar, const std::string& address);

	int execute(Process& process);

	std::string toString() const;

private:
	std::string targetVar;  // Name of the variable to read
	std::string hexAddress;    // Hexadecimal address as a string
	uint16_t address;        // Memory address to read from

	uint16_t variableValue; // Value read from the variable

	bool parseHexAddress(const std::string& addr, uint16_t& out) const; // Helper to parse hex address into an integer
};