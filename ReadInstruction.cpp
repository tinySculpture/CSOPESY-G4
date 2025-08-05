#include "ReadInstruction.h"

ReadInstruction::ReadInstruction(const std::string& targetVar, const std::string& address)
	: targetVar(targetVar), hexAddress(address) {
	if (!parseHexAddress(address, this->address)) {
		throw std::invalid_argument("Invalid hex address format: " + address);
	}
}

int ReadInstruction::execute(Process& process) {
    uint64_t total_virtual = process.virtual_size();
    if (address + 1 >= total_virtual) {
        // access violation
        process.record_access_violation(hexAddress);
        return -3;
    }

    // Translate first byte (might cross page boundary)
	uint64_t frame_size = MemoryManager::getInstance()->frame_size();
    uint64_t page_size = MemoryManager::getInstance()->frame_size();
    size_t page0 = static_cast<size_t>(address / page_size);
    size_t offset0 = static_cast<size_t>(address % page_size);

    // Ensure page0 resident
    PageTableEntry& pte0 = process.page_table_at(page0);
    if (!pte0.present) {
        MemoryManager::getInstance()->handle_page_fault(process, page0, false);
    }

    // Now safe to read two bytes
    // Get physical addresses
    auto read = [&](size_t virtual_addr)->uint16_t {
        size_t pg = static_cast<size_t>(virtual_addr / page_size);
        size_t off = static_cast<size_t>(virtual_addr % page_size);
        PageTableEntry& pte = process.page_table_at(pg);
        uint32_t frame = pte.frameNumber;
        // Update referenced
        pte.referenced = true;
        // Update frame info if needed (could add method)
        return MemoryManager::getInstance()->get_value(frame, off);
    };

    variableValue = read(address);

	process.setVariable(targetVar, variableValue);

	std::cout << "Read " << targetVar << " from " << hexAddress << " = " << process.getVariable(targetVar) << std::endl;

    process.addLog({
        ConsoleUtil::generateTimestamp(),
        process.getCoreID(),
        "Read " + targetVar + " from " + hexAddress + " = " + std::to_string(variableValue)
		});

	return 0; // No delay for read operation
}

std::string ReadInstruction::toString() const {
    return "READ FROM " + hexAddress;
}

bool ReadInstruction::parseHexAddress(const std::string& addr, uint16_t& out) const {
    try {
        out = static_cast<uint16_t>(std::stoul(addr, nullptr, 16));
        return true;
    } catch (const std::invalid_argument&) {
        return false; // Invalid hex format
    } catch (const std::out_of_range&) {
        return false; // Out of range for uint16_t
    }
}