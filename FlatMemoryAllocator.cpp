#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(const SystemConfig& config) {
	maximumSize = config.maxOverallMemory;
	allocatedSize = 0;
	frameSize = config.memoryPerFrame;
	numFrames = maximumSize / frameSize;

	memoryPool.resize(maximumSize);
	allocations.resize(numFrames);
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
	allocatedSize = 0;
}

size_t FlatMemoryAllocator::allocate(size_t size) {
	if (size == 0 || size > maximumSize - allocatedSize) {
		return -1; // Invalid size or not enough memory
	}

	// Figure out how many whole frames are needed
	size_t framesNeeded = (size + frameSize - 1) / frameSize; // Round up to nearest frame
	size_t neededSize = framesNeeded * frameSize;

	// Find the first available block that can accommodate the process
	for (size_t i = 0; i + framesNeeded <= numFrames; ++i) {
		size_t base = i * frameSize;
		if (!allocations[i] && canAllocateAt(base, neededSize)) {
			allocateAt(base, neededSize);
			return base;
		}
	}

	return -1; // No suitable block found
}

void FlatMemoryAllocator::deallocate(size_t base) {
	// 1) Out‑of‑bounds or not frame‑aligned ⇒ no‑op
	if (base >= maximumSize || base % frameSize != 0)
		return;

	// 2) Only deallocate if we actually have a block starting here
	auto it = blockMap.find(base);
	if (it == blockMap.end())
		return;

	// 3) Free the whole block
	deallocateAt(base);
}

void FlatMemoryAllocator::visualizeMemory() {
	// 1) Build filename
	std::string fname = "memory_stamp_" + std::to_string(GlobalScheduler::getInstance()->getNumCPUCycles()) + ".txt";
	std::ofstream ofs(fname);
	if (!ofs.is_open()) return;

	// 2) Timestamp
	ofs << "Timestamp: (" << ConsoleUtil::generateTimestamp() << ")\n";

	// 3) Count processes actually in memory (those whose base offset is in blockMap)
	size_t inMem = 0;
	auto processes = GlobalScheduler::getInstance()->getAllProcesses();
	for (auto& p : processes) {
		// assume p->getAllocationBase() returns the start byte if allocated, or -1 if none
		auto base = p->getAllocationBase();
		if (base >= 0 && blockMap.count(base)) ++inMem;
	}
	ofs << "Number of processes in memory: " << inMem << "\n";

	// 4) External fragmentation = free bytes total
	size_t freeBytes = maximumSize - allocatedSize;
	if (freeBytes == maximumSize)
		ofs << "Total external fragmentation in KB: " << 0 << "\n\n";
	else
		ofs << "Total external fragmentation in KB: " << freeBytes << "\n\n";

	// 5) Print the ASCII “map” of processes + holes, top→bottom
	ofs << "----end---- = " << maximumSize << "\n\n";

	// gather (base, name, size) triples
	struct Entry { size_t base; size_t size; std::string name; };
	std::vector<Entry> list;
	for (auto& p : processes) {
		size_t base = p->getAllocationBase();
		if (base != -1) {
			auto it = blockMap.find(base);
			Entry e = { base, it->second, p->getName() };
			if (it != blockMap.end()) {
				list.push_back(e);
			}
		}
	}
	// sort by base descending so we print from high addresses downward
	std::sort(list.begin(), list.end(),
		[&](auto& a, auto& b) { return a.base > b.base; });

	// current cursor in bytes
	size_t cursor = maximumSize;
	for (auto& e : list) {
		// hole above this block?
		if (e.base + e.size < cursor) {
			// print hole at [e.base+e.size .. cursor)
			ofs << cursor - 1  << "\n";
			ofs << "(free)" << "\n";
			ofs << (e.base + e.size) << "\n\n";
		}
		// then the block itself
		ofs << (e.base + e.size - 1) << "\n";
		ofs << e.name << "\n";
		ofs << e.base << "\n\n";
		cursor = e.base;
	}

	// final hole from 0 .. cursor?
	if (cursor > 0) {
		ofs << cursor << "\n";
		ofs << "(free)" << "\n";
		ofs << 0 << "\n";
	}

	ofs << "----start---- = 0\n";
	ofs.close();
}

void FlatMemoryAllocator::initializeMemory() {
	for (size_t i = 0; i < memoryPool.size(); ++i) {
		memoryPool[i] = '.'; // Initialize memory pool with null characters
	}

	for (size_t i = 0; i < allocations.size(); ++i) {
		allocations[i] = false; // Initialize all allocations as free
	}
}

bool FlatMemoryAllocator::canAllocateAt(size_t byteIndex, size_t size) const {
	// Check if the index is within bounds
	if (byteIndex % frameSize != 0) return false;

	// Compute how many frames we’d actually consume (ceiling)
	size_t framesNeeded = (size + frameSize - 1) / frameSize;
	size_t startFrame = byteIndex / frameSize;

	// Must not exceed total frames
	if (startFrame + framesNeeded > numFrames)
		return false;

	// Check each frame for free/allocated
	for (size_t f = startFrame; f < startFrame + framesNeeded; ++f) {
		auto it = allocations[f];
		if (it) {
			// this frame is already allocated
			return false;
		}
	}

	return true;
}

void FlatMemoryAllocator::allocateAt(size_t byteIndex, size_t size) {
	// Round up to whole frames
	size_t framesNeeded = (size + frameSize - 1) / frameSize;
	size_t allocBytes = framesNeeded * frameSize;

	// Record the allocation size by its byte‐index key
	blockMap[byteIndex] = allocBytes;

	// Mark each frame as allocated and fill the pool
	size_t startFrame = byteIndex / frameSize;
	for (size_t f = 0; f < framesNeeded; ++f) {
		size_t frameByteStart = (startFrame + f) * frameSize;
		allocations[startFrame + f] = true;
		// mark each byte in this frame
		for (size_t b = 0; b < frameSize; ++b) {
			memoryPool[frameByteStart + b] = '#';
		}
	}

	allocatedSize += allocBytes;
}

void FlatMemoryAllocator::deallocateAt(size_t byteIndex) {
	// Look up how many bytes we actually allocated here
	auto it = blockMap.find(byteIndex);
	if (it == blockMap.end()) return;

	size_t allocBytes = it->second;
	size_t framesUsed = allocBytes / frameSize;
	size_t startFrame = byteIndex / frameSize;

	// Free each frame and clear the pool bytes
	for (size_t f = 0; f < framesUsed; ++f) {
		allocations[startFrame + f] = false;
		size_t frameByteStart = (startFrame + f) * frameSize;
		for (size_t b = 0; b < frameSize; ++b) {
			memoryPool[frameByteStart + b] = '.';
		}
	}

	allocatedSize -= allocBytes;
	blockMap.erase(it);
}