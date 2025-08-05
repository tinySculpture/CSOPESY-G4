#include "Process.h"
#include "MemoryManager.h"
#include "ConsoleUtil.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <filesystem>

std::shared_ptr<MemoryManager> MemoryManager::instance = nullptr;

// Constructor for MemoryManager, initializes memory and frame table based on config
MemoryManager::MemoryManager(const SystemConfig& config) {
    memorySize = config.maxOverallMemory;      // Total memory size
    frameSize = config.memoryPerFrame;         // Size of each frame
    totalFrames = memorySize / frameSize;      // Calculate total number of frames

    frameTable.resize(totalFrames);            // Initialize frame table
    memory.resize(memorySize, 0);              // Initialize memory with zeros
    std::filesystem::create_directory("backing_store"); // Ensure backing store directory exists
}

// Static method to initialize the singleton instance
void MemoryManager::initialize(const SystemConfig& config) {
    if (!instance)
        instance = std::shared_ptr<MemoryManager>(new MemoryManager(config));
}

// Returns the singleton instance of MemoryManager
std::shared_ptr<MemoryManager> MemoryManager::getInstance() {
    return instance;
}

// Allocates a page table for the given process
void MemoryManager::allocatePageTable(std::shared_ptr<Process> process) {
    // If process requires more memory than available, block it
    if (process->getMemoryRequired() > memorySize) {
        process->setState(ProcessState::Blocked);
        return;
    }

    // Calculate number of pages needed for the process
    uint32_t pageCount = (process->getMemoryRequired() + frameSize - 1) / frameSize;
    process->initPageTable(pageCount); // Initialize the page table with the calculated size
}

// Handles memory access for a process at a given virtual address, with optional write flag
std::optional<uint32_t> MemoryManager::accessMemory(std::shared_ptr<Process> process, uint32_t virtualAddress, bool write) {
    // Check if the virtual address is within the process's memory bounds
    if (virtualAddress + 1 >= process->getMemoryRequired()) {
        ConsoleUtil::logError("Memory access out of process bounds. PID: " + std::to_string(process->getPID()));
        return std::nullopt;
    }

    // Calculate virtual page number (vpn) and offset within the page
    uint32_t vpn = virtualAddress / frameSize;
    uint32_t offset = virtualAddress % frameSize;

    auto& pageTable = process->getPageTable();

    // Check if the vpn is valid for this process
    if (vpn >= pageTable.size())
        return std::nullopt;

    PageTableEntry& entry = pageTable[vpn];

    // If the page is not currently loaded (not valid)
    if (!entry.valid) {
        // Count how many frames this process currently owns
        size_t framesOwned = 0;
        for (const auto& frame : frameTable) {
            if (frame.inUse && frame.pfid == process->getPID()) {
                ++framesOwned;
            }
        }

        // Calculate the maximum number of frames this process is allowed
        size_t maxAllowedFrames = process->getMemoryRequired() / frameSize;
        // If the process already owns the max allowed frames or not enough frames in system, block it
        if (framesOwned >= maxAllowedFrames || totalFrames < maxAllowedFrames) {
            process->setState(ProcessState::Blocked);
            return std::nullopt;
        }

        // Try to find a free frame
        int frameNumber = findFreeFrame();
        if (frameNumber == -1) {
            // If no free frame, evict a page using FIFO
            evictPage();
            frameNumber = findFreeFrame();
        }

        // If still no free frame, block the process
        if (frameNumber == -1) {
            process->setState(ProcessState::Blocked);
            return std::nullopt;
        }

        // Load the required page into the found frame
        loadPage(process, vpn, frameNumber);
        tryUnblockingBlockedProcesses(); // Try to unblock any processes that may now have enough memory
    }

    // Update the page table entry if this is a write operation
    PageTableEntry& updatedEntry = pageTable[vpn];
    if (write) updatedEntry.dirty = true;

    // Return the physical address corresponding to the virtual address
    return updatedEntry.frameNumber * frameSize + offset;
}

// Finds and returns the index of a free frame, or -1 if none are available
int MemoryManager::findFreeFrame() {
    for (size_t i = 0; i < frameTable.size(); ++i) {
        if (!frameTable[i].inUse)
            return static_cast<int>(i);
    }
    return -1;
}

// Evicts a page from memory using FIFO page replacement
void MemoryManager::evictPage() {
    if (fifoQueue.empty()) {
        std::cerr << "[X] FIFO queue is empty. Cannot evict any pages.\n";
        return;
    }

    // Get the oldest page (front of the queue)
    auto [pid, vpn] = fifoQueue.front();
    fifoQueue.pop_front();

    // Get the process by PID
    std::shared_ptr<Process> process = Process::getProcessByPID(pid);
    if (!process) return;

    auto& pageTable = process->getPageTable();
    if (vpn >= pageTable.size()) return;

    PageTableEntry& entry = pageTable[vpn];

    // If the page is valid (present in memory)
    if (entry.valid) {
        // If the page is dirty, save it to backing store before eviction
        if (entry.dirty) {
            savePageToBackingStore(pid, vpn, entry.frameNumber);
            ++pagesPagedOut;
        }
        // Mark the frame as free and invalidate the page table entry
        frameTable[entry.frameNumber].inUse = false;
        entry.valid = false;
    }
}

// Loads a page from backing store into a frame
void MemoryManager::loadPage(std::shared_ptr<Process> process, uint32_t vpn, uint32_t frameNumber) {
    // Load the page data from backing store (or initialize if not present)
    loadPageFromBackingStore(process->getPID(), vpn, frameNumber);

    ++pagesPagedIn;

    // Update the page table entry
    PageTableEntry& entry = process->getPageTable()[vpn];
    entry.valid = true;
    entry.frameNumber = frameNumber;
    entry.dirty = false;

    // Update the frame table to reflect the new mapping
    frameTable[frameNumber].inUse = true;
    frameTable[frameNumber].pfid = process->getPID();
    frameTable[frameNumber].virtualPageNumber = vpn;

    // Add this page to the FIFO queue for future eviction
    fifoQueue.emplace_back(process->getPID(), vpn);
}

void MemoryManager::loadPageFromBackingStore(uint32_t pid, uint32_t vpn, uint32_t frameNumber) {
    const std::string filename = "csopesy-backing-store.txt";
    const std::string header = "[PID " + std::to_string(pid) + " VPN " + std::to_string(vpn) + "]";
    const std::string footer = "[/PID " + std::to_string(pid) + " VPN " + std::to_string(vpn) + "]";
    const uint32_t physicalAddress = frameNumber * frameSize;

    std::ifstream inFile(filename);
    bool found = false;

    if (inFile) {
        std::string line;
        // Search for the header marking the start of the page data
        while (std::getline(inFile, line)) {
            if (line == header) {
                found = true;
                // Read each line as a byte of the page, until footer or frameSize is reached
                for (size_t i = 0; i < frameSize && std::getline(inFile, line); ++i) {
                    if (line == footer) break; // Stop if footer is reached early
                    memory[physicalAddress + i] = static_cast<uint8_t>(std::stoi(line));
                }
                break;
            }
        }
        inFile.close();
    }

    if (!found) {
        // If the page was not found in the backing store, initialize with zeros
        std::fill(memory.begin() + physicalAddress, memory.begin() + physicalAddress + frameSize, 0);
        // Save the newly initialized page to the backing store for future use
        savePageToBackingStore(pid, vpn, frameNumber);  
    }
}

void MemoryManager::savePageToBackingStore(uint32_t pid, uint32_t vpn, uint32_t frameNumber) {
    const std::string filename = "csopesy-backing-store.txt";
    const std::string header = "[PID " + std::to_string(pid) + " VPN " + std::to_string(vpn) + "]";
    const std::string footer = "[/PID " + std::to_string(pid) + " VPN " + std::to_string(vpn) + "]";
    const uint32_t physicalAddress = frameNumber * frameSize;

    std::ifstream inFile(filename);
    std::stringstream buffer;
    bool replaced = false;
    bool skip = false;

    if (inFile) {
        std::string line;
        // Read the file line by line
        while (std::getline(inFile, line)) {
            if (line == header) {
                // Found the header for this page, so we will replace this section
                skip = true;
                replaced = true;
                buffer << header << "\n";
                // Write the current page data from memory to the buffer
                for (size_t i = 0; i < frameSize; ++i)
                    buffer << std::to_string(memory[physicalAddress + i]) << "\n";
                buffer << footer << "\n";

                // Skip lines until the footer is found
                while (std::getline(inFile, line)) {
                    if (line == footer) {
                        skip = false;
                        break;
                    }
                }
                continue;
            }
            if (!skip) {
                // Copy lines not being replaced
                buffer << line << "\n";
            }
        }
        inFile.close();
    }

    if (!replaced) {
        // If the page was not found, append it at the end
        buffer << header << "\n";
        for (size_t i = 0; i < frameSize; ++i)
            buffer << std::to_string(memory[physicalAddress + i]) << "\n";
        buffer << footer << "\n";
    }

    // Write the updated buffer back to the file
    std::ofstream outFile(filename);
    outFile << buffer.str();
    outFile.close();
}

// Reads a 16-bit unsigned integer from the given physical address in memory
uint16_t MemoryManager::readUint16At(uint32_t physicalAddress) {
    // Check for out-of-bounds access
    if (physicalAddress + 1 >= memory.size()) {
        throw std::out_of_range("Attempted to read beyond memory bounds");
    }

    // Combine two consecutive bytes into a uint16_t (little-endian)
    uint16_t value = static_cast<uint16_t>(memory[physicalAddress])
                   | (static_cast<uint16_t>(memory[physicalAddress + 1]) << 8);
    return value;
}

// Writes a 16-bit unsigned integer to the given physical address in memory
void MemoryManager::writeUint16At(uint32_t physicalAddress, uint16_t value) {
    // Check for out-of-bounds access
    if (physicalAddress + 1 >= memory.size()) {
        throw std::out_of_range("Attempted to write beyond memory bounds");
    }

    // Store the lower and upper bytes (little-endian)
    memory[physicalAddress]     = static_cast<uint8_t>(value & 0xFF);
    memory[physicalAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

// Frees all pages/frames owned by the process with the given PID
void MemoryManager::freeProcessPages(uint32_t pid) {
    // Mark all frames owned by this process as free
    for (auto& frame : frameTable) {
        if (frame.inUse && frame.pfid == pid) {
            frame.inUse = false;
            frame.pfid = -1;
            frame.virtualPageNumber = -1;
        }
    }

    // Invalidate all page table entries for this process
    auto it = Process::pidToProcess.find(pid);
    if (it != Process::pidToProcess.end()) {
        auto process = it->second;
        for (auto& entry : process->getPageTable()) {
            entry.valid = false;
            entry.frameNumber = -1;
            entry.dirty = false;
        }
    }

    // Remove all entries for this process from the FIFO queue
    fifoQueue.erase(
        std::remove_if(fifoQueue.begin(), fifoQueue.end(),
                       [pid](const std::pair<uint32_t, uint32_t>& p) {
                           return p.first == pid;
                       }),
        fifoQueue.end()
    );

    // Try to unblock any processes that may now have enough memory
    tryUnblockingBlockedProcesses();
}

// Checks if a blocked process can be unblocked (has room for more frames and a free frame exists)
bool MemoryManager::canUnblock(std::shared_ptr<Process> process) {
    size_t framesOwned = 0;
    // Count frames currently owned by the process
    for (const auto& frame : frameTable) {
        if (frame.inUse && frame.pfid == process->getPID())
            ++framesOwned;
    }

    // Calculate the maximum frames allowed for this process
    size_t maxAllowedFrames = process->getMemoryRequired() / frameSize;
    bool hasRoomForMoreFrames = framesOwned < maxAllowedFrames;
    bool hasFreeFrame = findFreeFrame() != -1;

    // Can unblock if process can own more frames and a free frame exists
    return hasRoomForMoreFrames && hasFreeFrame;
}

// Attempts to unblock all blocked processes that can now proceed
void MemoryManager::tryUnblockingBlockedProcesses() {
    for (auto& [pid, process] : Process::pidToProcess) {
        if (process->getState() == ProcessState::Blocked &&
            canUnblock(process)) {
            process->setState(ProcessState::Ready);
            GlobalScheduler::getInstance()->addProcess(process);
        }
    }
}