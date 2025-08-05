#include "PhysicalMemory.h"
#include <stdexcept>

PhysicalMemory::PhysicalMemory(uint32_t numFrames_, uint32_t pageSize_)
    : numFrames(numFrames_), pageSize(pageSize_) {
    frames.reserve(numFrames);
    for (uint32_t i = 0; i < numFrames; ++i) {
		frames.emplace_back(std::make_shared<Frame>(pageSize));
    }
}

std::optional<uint32_t> PhysicalMemory::findFreeFrameIndex() const {
    std::lock_guard<std::mutex> lock(mutex);
    for (uint32_t i = 0; i < numFrames; ++i) {
        if (frames[i]->isFree()) return i;
    }
    return std::nullopt;
}

std::optional<uint32_t> PhysicalMemory::assignPage(int processID,
    uint32_t pageNumber,
    const std::vector<uint8_t>& content) {

    std::lock_guard<std::mutex> lock(mutex);
    auto freeOpt = findFreeFrameIndex();
    if (!freeOpt.has_value()) return std::nullopt;

    uint32_t frameIdx = freeOpt.value();
    auto frame = frames[frameIdx];
    frame->loadContents(content);
    frame->setOwner(processID, pageNumber);
    frame->markDirty(false); // fresh load
    return frameIdx;
}

std::shared_ptr<Frame> PhysicalMemory::getFrame(uint32_t frameNumber) {
    if (frameNumber >= numFrames) {
        throw std::out_of_range("PhysicalMemory::getFrame: invalid frame number");
    }
    std::lock_guard<std::mutex> lock(mutex);
    return frames[frameNumber];
}

uint32_t PhysicalMemory::getNumFrames() const {
    return numFrames;
}

uint32_t PhysicalMemory::getPageSize() const {
    return pageSize;
}

std::shared_ptr<Frame> PhysicalMemory::resetFrame(uint32_t frameNumber) {
    std::lock_guard<std::mutex> lock(mutex);
    if (frameNumber >= numFrames) {
        throw std::out_of_range("PhysicalMemory::resetFrame: invalid frame number");
    }

    auto previous = std::move(frames[frameNumber]); // copy current state
    frames[frameNumber]->clear();          // reset in-place

    return std::move(previous);
}
