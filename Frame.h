#pragma once

#include <cstdint>
#include <vector>
#include <string>

/**
 * Descriptor for who owns a frame.
 */
struct FrameOwner {
    int processID = -1;                // -1 means unassigned / free
    uint32_t pageNumber = UINT32_MAX;  // virtual page number
};

/**
 * Represents one physical frame (i.e., one page-sized slot in RAM).
 * Does not implement replacement policy; that lives outside (e.g., FIFO queue).
 */
class Frame {
public:
    Frame(uint32_t frameNumber, uint32_t pageSize);

    // Ownership
    void setOwner(int processID, uint32_t pageNumber);
    FrameOwner getOwner() const;
    void resetOwner(); // mark as free
    bool isFree() const;

    // Dirty tracking
    void markDirty(bool d = true);
    bool isDirty() const;

    // Identity
    uint32_t getFrameNumber() const;

    // Data access (caller is responsible for any external synchronization)
    std::vector<uint8_t>& data();
    const std::vector<uint8_t>& data() const;

    // Reset content and metadata
    void clear();

    // Serialization for backing store (space-separated decimal bytes)
    std::string serializeContents() const;

    // Load raw contents (must match pageSize)
    void loadContents(const std::vector<uint8_t>& src);

private:
    FrameOwner owner;
    bool dirty = false;
    uint32_t frameNumber;
    std::vector<uint8_t> content; // size == pageSize
};
