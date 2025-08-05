#pragma once

#include <cstdint>
#include <vector>

#include "SystemConfig.h"

/**
 * Virtual address: page number + offset inside page.
 */
struct VirtualAddress {
    uint32_t pageNumber;
    uint32_t offset; // must be < PAGE_SIZE
};

/**
 * Physical address: frame number + offset.
 */
struct PhysicalAddress {
    uint32_t frameNumber;
    uint32_t offset;
};

/**
 * Symbol table entry: variable name mapped to a virtual address and size.
 */
struct SymbolEntry {
    VirtualAddress vaddr;
    uint32_t sizeBytes; // e.g., 2 for uint16_t
};

/**
 * Ownership metadata for physical frames, used for eviction bookkeeping.
 */
struct FrameOwner {
    int processID = -1;
    uint32_t pageNumber = UINT32_MAX;
};