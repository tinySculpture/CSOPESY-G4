#include "MemoryManager.h"
#include "BackingStore.h"
#include <iostream>
#include <cstring>
#include <cassert>


MemoryManager::MemoryManager() {
    m_total_mem = SystemConfig::getInstance()->maxOverallMemory;
	m_frame_size = SystemConfig::getInstance()->memoryPerFrame;
    m_frame_count = static_cast<size_t>(m_total_mem / m_frame_size);

    // Physical memory initialization: each frame holds frame_size bytes.
    m_physical_memory.resize(m_frame_count);
    for (int i = 0; i < m_frame_count; ++i) {
        m_physical_memory[i].assign(
            m_frame_size,
            std::make_pair(std::string(), uint16_t{ 0 })
        );
    }

    // Frame table
    m_frame_table.resize(m_frame_count);

    // All frames start free
    for (uint32_t i = 0; i < m_frame_count; ++i) {
        m_free_frames.push_back(i);
    }
}

void MemoryManager::initialize() {
    if (!sharedInstance) {
        sharedInstance = new MemoryManager();
    }
}

MemoryManager* MemoryManager::getInstance() {
    return sharedInstance;
}

void MemoryManager::destroy() {
    if (sharedInstance) {
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

bool MemoryManager::has_free_frame() const {
    return !m_free_frames.empty();
}

uint32_t MemoryManager::allocate_frame(uint32_t pid, size_t vpn, Process& proc, bool is_write) {
    // Called during page fault resolution.
    uint32_t frame = UINT32_MAX;
    if (has_free_frame()) {
        frame = m_free_frames.front();
        m_free_frames.pop_front();
    }
    else {
        frame = select_victim_frame();
    }

    load_page(proc, vpn, frame, is_write);
    return frame;
}

void MemoryManager::free_frame(uint32_t frame_number) {
    if (frame_number >= m_frame_table.size()) return;
    FrameInfo& info = m_frame_table[frame_number];
    // Reset metadata
    info = FrameInfo();

    // Remove from FIFO queue if present
    auto it = std::find(m_fifo_queue.begin(), m_fifo_queue.end(), frame_number);
    if (it != m_fifo_queue.end()) m_fifo_queue.erase(it);

    // Zero out physical memory for cleanliness
    m_physical_memory[frame_number].assign(
        m_frame_size,
        std::make_pair(std::string(), uint16_t{ 0 })
    );

    m_free_frames.push_back(frame_number);
}

void MemoryManager::handle_page_fault(Process& proc, size_t vpn, bool is_write) {
    // Validate VPN is in range (should be enforced earlier)
    if (vpn >= proc.num_pages()) {
        // Access violation: outside allocated virtual range; caller handles process termination.
        return;
    }

    ++proc.page_faults;

    // Attempt to allocate or evict and bring in page
    uint32_t frame = allocate_frame(proc.getPID(), vpn, proc, is_write);

    // Update process page table entry (done in load_page)
    (void)frame; // silence unused if needed
}

size_t MemoryManager::resident_frames_count() const {
    return m_frame_count - m_free_frames.size();
}

uint64_t MemoryManager::total_frames() const {
    return m_frame_count;
}

uint64_t MemoryManager::free_frames() const {
    return m_free_frames.size();
}

uint32_t MemoryManager::select_victim_frame() {
    // Global FIFO: front is oldest arrival_order
    if (m_fifo_queue.empty()) {
        throw std::runtime_error("No frame to evict but free list empty; inconsistent state.");
    }
    uint32_t victim = m_fifo_queue.front();
    m_fifo_queue.pop_front();

    FrameInfo& finfo = m_frame_table[victim];

    // Eviction logic
    if (finfo.dirty) {
        // Write back to backing store before reuse
        std::vector<std::pair<std::string, uint16_t>> page_data(m_frame_size);
		page_data = std::move(m_physical_memory[victim]);
        BackingStore::getInstance()->write_page(finfo.owner_pid, finfo.vpn, page_data);

        // NOW reset the frame back to a full, zeroed page:
        m_physical_memory[victim].assign(
            m_frame_size,
            std::make_pair(std::string(), uint16_t{ 0 })
        );

        ++evictions_dirty;
        ++total_pages_out;
    }
    else {
        ++evictions_clean;
        // Clean page: if it existed in backing store, we could discard; anonymous pages may not have stored version.
        // No write-back required.
    }

    // Invalidate victim's page table entry immediately
	std::shared_ptr<Process> victim_proc = nullptr;
    for (std::shared_ptr<Process> proc : processList) {
        if (proc->getPID() == finfo.owner_pid) {
            victim_proc = proc;
            break;
        }
	}

    if (victim_proc) {
        if (finfo.vpn < victim_proc->num_pages()) {
            PageTableEntry& victim_pte = victim_proc->page_table_at(finfo.vpn);
            victim_pte.present = false;
            victim_pte.frameNumber = UINT32_MAX;
            // Keep dirty/ref bits for potential metadata if needed.
        }
    }

    // Reset frame metadata (owner/vpn will be overwritten by load_page)
    finfo.dirty = false;
    finfo.referenced = false;
    finfo.owner_pid = 0;
    finfo.vpn = 0;
    finfo.arrival_order = 0;

    return victim;
}

void MemoryManager::load_page(Process& proc, size_t vpn, uint32_t frame_number, bool is_write) {
    // Load page content into physical frame.
    // If the page already exists in backing store, read it; else zero-filled (anonymous).

    // If the frame was previously occupied, the caller should have evicted it already.
    FrameInfo& finfo = m_frame_table[frame_number];

    ++total_pages_in;

    // Read existing contents if any
    std::vector<std::pair<std::string, uint16_t>> page_data;
    if (BackingStore::getInstance()->has_page(proc.getPID(), vpn)) {
        page_data = BackingStore::getInstance()->read_page(proc.getPID(), vpn);
    }
    else {
        // assign zero page to page data
        page_data.assign(
            m_frame_size,
            std::make_pair(std::string(), uint16_t{ 0 })
        );
    }

    // Copy data into physical memory
	m_physical_memory[frame_number] = std::move(page_data);

    // Update frame metadata
    finfo.owner_pid = proc.getPID();
    finfo.vpn = vpn;
    finfo.dirty = is_write; // if this fault was triggered by a write, we mark dirty; else clean
    finfo.referenced = true;
    finfo.arrival_order = m_next_arrival_counter++;

    // Update global FIFO queue: newest at back
    m_fifo_queue.push_back(frame_number);

    // Update process page table entry
    PageTableEntry& pte = proc.page_table_at(vpn);
    pte.present = true;
    pte.frameNumber = frame_number;
    pte.dirty = finfo.dirty;
    pte.referenced = true;
    pte.arrival_order = finfo.arrival_order;
}

void MemoryManager::addProcess(std::shared_ptr<Process> proc) {
    std::lock_guard<std::mutex> lock(processListMutex);
    processList.push_back(proc);
}