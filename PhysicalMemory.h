#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <mutex>
#include "Frame.h"

/**
 * Simple physical memory manager without replacement.
 * Handles allocation of virtual pages into free frames.
 */
class PhysicalMemory {
public:
    PhysicalMemory(uint32_t numFrames, uint32_t pageSize);

    /**
     * Assign a page (content) to any free frame.
     * @return frame number if assignment succeeded, std::nullopt if no free frame available.
     */
    std::optional<uint32_t> assignPage(int processID,
        uint32_t pageNumber,
        const std::vector<uint8_t>& content);

    /** @return an index of a free frame if any (does not reserve it). */
    std::optional<uint32_t> findFreeFrameIndex() const;

    /** Direct access to a frame; throws if invalid index. */
    std::shared_ptr<Frame> getFrame(uint32_t frameNumber);

    uint32_t getNumFrames() const;
    uint32_t getPageSize() const;

    /**
     * Reset the frame at frameNumber (clears its contents/metadata) and return its previous state.
     * Caller receives a copy of what was in the frame before the reset.
     */
    std::shared_ptr<Frame> resetFrame(uint32_t frameNumber);

private:
    uint32_t numFrames;
    uint32_t pageSize;
    std::vector<std::shared_ptr<Frame>> frames;
    mutable std::mutex mutex;

};
