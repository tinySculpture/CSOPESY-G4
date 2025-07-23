#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(const SystemConfig& config) {
	maximumSize = config.maxOverallMemory;
	memoryPool.reserve(maximumSize);
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
	stop();     // Ensure clean shutdown
}

void FlatMemoryAllocator::start() {
	initializeMemory();
}

void FlatMemoryAllocator::stop() {
	// Clear memory pool and allocations
	memoryPool.clear();
	allocations.clear();
}

void* FlatMemoryAllocator::allocate(size_t size) {
	// Find the first available block that can accommodate the process
	for (size_t i = 0; i < maximumSize - size + 1; ++i) {
		if (!allocations[i] && canAllocateAt(i, size)) {
			allocateAt(i, size);
			return &memoryPool[i];
		}
	}

	return nullptr; // No suitable block found
}

void FlatMemoryAllocator::deallocate(void* ptr) {
	size_t index = static_cast<char*>(ptr) - &memoryPool[0];
	if (allocations[index]) {
		deallocateAt(index);
	}
}

std::string FlatMemoryAllocator::visualizeMemory() {
	return std::string(memoryPool.begin(), memoryPool.end());
}

void FlatMemoryAllocator::initializeMemory() {
	std::fill(memoryPool.begin(), memoryPool.end(), ".");
	std::fill(allocations.begin(), allocations.end(), false);
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
	// Check if the block from index to index + size is free
	return (index + size <= maximumSize);
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
	// allocations is a std::unordered_map<size_t, bool>, so we can't use std::fill or pointer arithmetic.
	for (size_t i = index; i < index + size; ++i) {
		allocations[i] = true;
	}
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
	allocations[index] = false;
}