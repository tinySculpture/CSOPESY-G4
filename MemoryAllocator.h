#pragma once
#include <string>

class MemoryAllocator {
public:
	MemoryAllocator() {}
	virtual ~MemoryAllocator() = default;

	virtual void start() = 0;
	virtual void stop() = 0;

	virtual size_t allocate(size_t size) = 0;
	virtual void deallocate(size_t base) = 0;
	virtual void visualizeMemory() = 0;
};