#include "GlobalMemoryAllocator.h"

GlobalMemoryAllocator::GlobalMemoryAllocator(const SystemConfig& config) {
	// Instantiate the FlatMemoryAllocator
	allocators["flat"] = std::make_shared<FlatMemoryAllocator>(config);
	allocators["demandPaging"] = nullptr;
	
	// Set the current allocator to FlatMemoryAllocator by default
	currentAllocator = allocators["flat"];
}

void GlobalMemoryAllocator::initialize(const SystemConfig& config) {
	if (!sharedInstance)
		sharedInstance = new GlobalMemoryAllocator(config);
}

GlobalMemoryAllocator* GlobalMemoryAllocator::getInstance() {
	return sharedInstance;
}

void GlobalMemoryAllocator::destroy() {
	delete sharedInstance;
	sharedInstance = nullptr;
}

void GlobalMemoryAllocator::start() {
	running = true;
	if (currentAllocator) {
		currentAllocator->start();
	}
}

void GlobalMemoryAllocator::stop() {
	running = false;
	if (currentAllocator) {
		currentAllocator->stop();
	}
}

void* GlobalMemoryAllocator::allocate(size_t size) {
	if (currentAllocator) {
		return currentAllocator->allocate(size);
	}
	return nullptr; // No allocator available
}

void GlobalMemoryAllocator::deallocate(void* ptr) {
	if (currentAllocator) {
		currentAllocator->deallocate(ptr);
	}
}

std::string GlobalMemoryAllocator::visualizeMemory() {
	if (currentAllocator) {
		return currentAllocator->visualizeMemory();
	}
	return ""; // No allocator available
}

bool GlobalMemoryAllocator::isRunning() {
	return running;
}