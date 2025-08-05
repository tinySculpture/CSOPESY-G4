#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

#include "Process.h"
#include "SystemConfig.h"
#include "MemoryManager.h"
#include "ConsoleUtil.h"
#include "Instruction.h"

class WriteInstruction : public Instruction {
public:
	WriteInstruction(const std::string& address, uint16_t value);

	int execute(Process& process);

	std::string toString() const;

private:
	std::string hexAddress;    // Hexadecimal address as a string
	uint16_t address;        // Memory address to write to

	uint16_t value;	 // Value to write to the address

	bool parseHexAddress(const std::string& addr, uint16_t& out) const; // Helper to parse hex address into an integer
};