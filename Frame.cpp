#include "Frame.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

Frame::Frame(uint32_t frameNumber_, uint32_t pageSize)
    : frameNumber(frameNumber_), content(pageSize, 0) {
    // Initially free, clean
    owner = FrameOwner{ -1, UINT32_MAX };
    dirty = false;
}

void Frame::setOwner(int processID, uint32_t pageNumber) {
    owner.processID = processID;
    owner.pageNumber = pageNumber;
}

FrameOwner Frame::getOwner() const {
    return owner;
}

void Frame::resetOwner() {
    owner = FrameOwner{ -1, UINT32_MAX };
}

bool Frame::isFree() const {
    return owner.processID == -1;
}

void Frame::markDirty(bool d) {
    dirty = d;
}

bool Frame::isDirty() const {
    return dirty;
}

uint32_t Frame::getFrameNumber() const {
    return frameNumber;
}

std::vector<uint8_t>& Frame::data() {
    return content;
}

const std::vector<uint8_t>& Frame::data() const {
    return content;
}

void Frame::clear() {
    std::fill(content.begin(), content.end(), 0);
    dirty = false;
    resetOwner();
}

std::string Frame::serializeContents() const {
    std::ostringstream oss;
    for (size_t i = 0; i < content.size(); ++i) {
        oss << static_cast<int>(content[i]);
        if (i + 1 < content.size()) oss << ' ';
    }
    return oss.str();
}

void Frame::loadContents(const std::vector<uint8_t>& src) {
    if (src.size() != content.size()) {
        throw std::runtime_error("Frame::loadContents: size mismatch");
    }
    std::copy(src.begin(), src.end(), content.begin());
}
