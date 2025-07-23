#pragma once
#include <string>

class MemoryAllocator {
public:
	MemoryAllocator() {}
	virtual ~MemoryAllocator() = default;

	virtual void start() = 0;
	virtual void stop() = 0;

	virtual void* allocate(size_t size) = 0;
	virtual void deallocate(void* ptr) = 0;
	virtual std::string visualizeMemory() = 0;
};