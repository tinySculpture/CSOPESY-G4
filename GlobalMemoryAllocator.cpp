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

size_t GlobalMemoryAllocator::allocate(size_t size) {
	if (currentAllocator) {
		return currentAllocator->allocate(size);
	}
	return -2; // No allocator available
}

void GlobalMemoryAllocator::deallocate(size_t base) {
	if (currentAllocator) {
		currentAllocator->deallocate(base);
	}
}

void GlobalMemoryAllocator::visualizeMemory() {
	if (currentAllocator) {
		currentAllocator->visualizeMemory();
	}
	return;
}

bool GlobalMemoryAllocator::isRunning() {
	return running;
}