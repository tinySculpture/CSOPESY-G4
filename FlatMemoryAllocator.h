#pragma once

#include <vector>
#include <unordered_map>

#include "MemoryAllocator.h"
#include "SystemConfig.h"

class FlatMemoryAllocator : public MemoryAllocator {
public:
	FlatMemoryAllocator(const SystemConfig& config);
	~FlatMemoryAllocator();

	void start() override;
	void stop() override;

	void* allocate(size_t size) override;
	void deallocate(void* ptr) override;
	std::string visualizeMemory() override;

private:
	size_t maximumSize;  // Maximum size of the memory pool
	size_t allocatedSize; // Current allocated size
	std::vector<char> memoryPool; // Memory pool storage
	std::unordered_map<size_t, bool> allocations; // Track allocations and their sizes

	void initializeMemory();

	bool canAllocateAt(size_t index, size_t size) const;

	void allocateAt(size_t index, size_t size);

	void deallocateAt(size_t index);

}