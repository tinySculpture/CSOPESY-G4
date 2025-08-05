#include "WriteInstruction.h"

WriteInstruction::WriteInstruction(const std::string& address, uint16_t value)
	: hexAddress(address), value(value) {
	if (!parseHexAddress(address, this->address)) {
		throw std::invalid_argument("Invalid hex address format: " + address);
	}
}

int WriteInstruction::execute(Process& process) {
	uint64_t total_virtual = process.virtual_size();
	if (address + 1 >= total_virtual) {
		// access violation
		process.record_access_violation(hexAddress);
		return -3;
	}
	// Translate first byte (might cross page boundary)
	uint64_t page_size = MemoryManager::getInstance()->frame_size();
	size_t page0 = static_cast<size_t>(address / page_size);
	size_t offset0 = static_cast<size_t>(address % page_size);
	// Ensure page0 resident
	PageTableEntry& pte0 = process.page_table_at(page0);
	if (!pte0.present) {
		MemoryManager::getInstance()->handle_page_fault(process, page0, true);
	}
	// Now safe to write two bytes
	// Get physical addresses
	auto write = [&](size_t virtual_addr, uint16_t value)->void {
		size_t pg = static_cast<size_t>(virtual_addr / page_size);
		size_t off = static_cast<size_t>(virtual_addr % page_size);
		PageTableEntry& pte = process.page_table_at(pg);
		uint32_t frame = pte.frameNumber;
		// Update referenced and dirty bits
		pte.referenced = true;
		pte.dirty = true;
		// Update frame info if needed (could add method)
		MemoryManager::getInstance()->set_value(frame, off, value);
	};

	write(address, value);

	process.setVariable("var" + std::to_string(address), value); // Store in variable table

	std::cout << "Write " << value << " to " << hexAddress << std::endl;

	process.addLog({
		ConsoleUtil::generateTimestamp(),
		process.getCoreID(),
		"Write " + std::to_string(value) + " to " + hexAddress
	});
	return 0; // No delay for write operation
}


std::string WriteInstruction::toString() const {
	return "WRITE TO " + hexAddress + " = " + std::to_string(value);
}

bool WriteInstruction::parseHexAddress(const std::string& addr, uint16_t& out) const {
	try {
		out = static_cast<uint16_t>(std::stoul(addr, nullptr, 16));
		return true;
	} catch (const std::invalid_argument&) {
		return false; // Invalid hex format
	} catch (const std::out_of_range&) {
		return false; // Out of range for uint16_t
	}
}