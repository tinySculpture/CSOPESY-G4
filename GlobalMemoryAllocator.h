#pragma once
#include <memory>
#include <unordered_map>

#include "SystemConfig.h"
#include "FlatMemoryAllocator.h"

class MemoryAllocator;

class GlobalMemoryAllocator {
public:
    static void initialize(const SystemConfig& config);

    static GlobalMemoryAllocator* getInstance();

    static void destroy();

    void start();
    void stop();

	void* allocate(size_t size);
	void deallocate(void* ptr);
	std::string visualizeMemory();

    bool isRunning();

private:
    GlobalMemoryAllocator(const SystemConfig& config);

    inline static GlobalMemoryAllocator* sharedInstance = nullptr;    // Singleton instance
    bool running = false;                                       // Scheduler running state

    std::unordered_map<std::string, std::shared_ptr<MemoryAllocator>> allocators;     // Available schedulers
    std::shared_ptr<MemoryAllocator> currentAllocator;                                // Active Scheduler
};