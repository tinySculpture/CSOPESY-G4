#pragma once

#include <memory>
#include <deque>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <optional>
#include <mutex>

#include "SystemConfig.h"

struct PageTableEntry {
    bool valid;
    bool dirty;
    uint32_t frameNumber;
};

struct PageFrame {
    int pfid;
    uint32_t virtualPageNumber;
    bool inUse = false;
};

class Process;

class MemoryManager {
public:
    static void initialize(const SystemConfig& config);
    static std::shared_ptr<MemoryManager> getInstance();

    void allocatePageTable(std::shared_ptr<Process> process);
    std::optional<uint32_t> accessMemory(std::shared_ptr<Process> process, uint32_t virtualAddress, bool write);

    uint16_t readUint16At(uint32_t physicalAddress);
    void writeUint16At(uint32_t physicalAddress, uint16_t value);

    uint32_t getMemorySize() const { return memorySize; }
    uint32_t getFrameSize() const { return frameSize; }
    uint32_t getTotalFrames() const { return totalFrames; }
    const std::vector<PageFrame>& getFrameTable() const { return frameTable; }
    uint32_t getPagesPagedIn() const { return pagesPagedIn; }
    uint32_t getPagesPagedOut() const { return pagesPagedOut; }

    void freeProcessPages(uint32_t pid);
    bool canUnblock(std::shared_ptr<Process> process);
    void tryUnblockingBlockedProcesses();

private:
    MemoryManager(const SystemConfig& config);

    int findFreeFrame();
    void evictPage();
    void loadPage(std::shared_ptr<Process> process, uint32_t vpn, uint32_t frameNumber);
    void savePageToBackingStore(uint32_t pid, uint32_t vpn, uint32_t frameNumber);
    void loadPageFromBackingStore(uint32_t pid, uint32_t vpn, uint32_t frameNumber);

    std::vector<uint8_t> memory;

    uint32_t memorySize;
    uint32_t frameSize;
    uint32_t totalFrames;

    uint32_t pagesPagedIn = 0;
    uint32_t pagesPagedOut = 0;

    std::vector<PageFrame> frameTable;
    std::deque<std::pair<uint32_t, uint32_t>> fifoQueue;

    static std::shared_ptr<MemoryManager> instance;
};