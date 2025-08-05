#pragma once

#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstddef>
#include <map>

#include "Process.h"
#include "BackingStore.h"
#include "SystemConfig.h"

struct FrameInfo {
    uint32_t owner_pid = 0;          // PID of owning process
    size_t vpn = 0;                  // virtual page number
    bool dirty = false;              // dirty bit
    bool referenced = false;         // referenced bit (can be used for future enhancements)
    uint64_t arrival_order = 0;      // for FIFO ordering (lower = older)
};

/**
 * Forward declaration of BackingStore.
 */
class BackingStore;

class MemoryManager {
public:
    static void initialize();

    static MemoryManager* getInstance();
    
    static void destroy();

    /**
     * Returns whether any free frame exists.
     */
    bool has_free_frame() const;

    /**
     * Allocates (or reuses via eviction) a frame for (pid, vpn) on demand.
     * Returns physical frame number assigned.
     * Updates internal structures and handles eviction if needed.
     * MO2 spec: demand paging + FIFO replacement.
     */
    uint32_t allocate_frame(uint32_t pid, size_t vpn, Process& proc, bool is_write);

    /**
     * Frees a frame explicitly (e.g., when a process terminates).
     */
    void free_frame(uint32_t frame_number);

    /**
     * Handles a page fault for a given process and virtual page number.
     * is_write indicates if the faulting access is a write (for dirty semantics).
     * After this returns, the page should be resident and page table updated.
     * MO2 spec: Section 4.2 – Page fault handling and restart logic.
     */
    void handle_page_fault(Process& proc, size_t vpn, bool is_write);

    /**
     * Total number of resident frames currently in use.
     */
    size_t resident_frames_count() const;

    uint64_t total_frames() const;
    uint64_t free_frames() const;

    void addProcess(std::shared_ptr<Process> proc);

    // Statistics
    uint64_t total_pages_in = 0;         // paged-in count
    uint64_t total_pages_out = 0;        // paged-out count
    uint64_t evictions_clean = 0;
    uint64_t evictions_dirty = 0;

    // returns frame size
    uint64_t frame_size() const {
        return m_frame_size;
	}

    // get physical memory
    uint16_t get_value(uint32_t frame_number, size_t offset) const {
        if (frame_number >= m_frame_table.size() || offset >= m_frame_size) {
            throw std::out_of_range("Invalid frame number or offset");
        }
        return m_physical_memory[frame_number].at(offset).second;
	}

	// set physical memory
    void set_value(uint32_t frame_number, size_t offset, uint16_t value) {
        if (frame_number >= m_frame_table.size() || offset >= m_frame_size) {
            throw std::out_of_range("Invalid frame number or offset");
        }
        m_physical_memory[frame_number].at(offset).second = value;
    }

private:
    /**
     * Constructor. Initializes frames, free list, and ties to backing store.
     * MO2 spec: Section 4 – Virtual Memory, Paging, and Backing Store
     */
    MemoryManager();

	inline static MemoryManager* sharedInstance = nullptr;

    uint64_t m_total_mem;
    uint64_t m_frame_size;
    size_t m_frame_count;

	std::mutex processListMutex; // Guards processList access
	std::vector<std::shared_ptr<Process>> processList; // Current process being served (if any)

    // Simulated physical memory: two bytes per entry
    std::vector<std::vector<std::pair<std::string, uint16_t>>> m_physical_memory;

    // Free frame list
    std::deque<uint32_t> m_free_frames;

    // Frame table metadata
    std::vector<FrameInfo> m_frame_table;

    // Global FIFO queue of frame numbers (oldest at front)
    std::deque<uint32_t> m_fifo_queue;

    // Arrival counter increments with each load to give total ordering.
    uint64_t m_next_arrival_counter = 1;

    /**
     * Selects a victim frame using global FIFO policy.
     * Evicts it (write back if dirty) and returns the frame number.
     */
    uint32_t select_victim_frame();

    /**
     * Loads the page into the provided frame (either a freed frame or victim) for the process.
     */
    void load_page(Process& proc, size_t vpn, uint32_t frame_number, bool is_write);
};
