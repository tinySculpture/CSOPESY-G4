#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "MemoryAllocator.h"
#include "SystemConfig.h"
#include "Process.h"
#include "ConsoleUtil.h"
#include "GlobalScheduler.h"

class FlatMemoryAllocator : public MemoryAllocator {
public:
	FlatMemoryAllocator(const SystemConfig& config);
	~FlatMemoryAllocator();

	void start() override;
	void stop() override;

	size_t allocate(size_t size) override;
	void deallocate(size_t base) override;
	void visualizeMemory() override;

private:
	size_t maximumSize;  // Maximum size of the memory pool
	size_t allocatedSize; // Current allocated size
	size_t frameSize;     // Size of each memory frame
	size_t numFrames;     // Total number of frames in the memory pool
	std::vector<char> memoryPool; // Memory pool storage
	std::vector<bool> allocations; // Track allocations and their sizes
	std::unordered_map<size_t, size_t> blockMap; // Track sizes of allocations

	void initializeMemory();

	bool canAllocateAt(size_t index, size_t size) const;

	void allocateAt(size_t index, size_t size);

	void deallocateAt(size_t index);

};