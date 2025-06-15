#pragma once
#include "Process.h"

// Abstract base class for different CPU scheduling algorithms
class Scheduler {
public:
    /**
     * Enqueue a new process into the scheduler.
     * @param process Pointer to the process to schedule.
     */
    virtual void addProcess(const Process* process) = 0;

    /**
     * Start the scheduling mechanism (spawn threads, begin dispatching).
     */
    virtual void start() = 0;

    /**
     * Stop scheduling, wait for all tasks to complete, and clean up threads.
     */
    virtual void stop() = 0;

    /**
     * Virtual destructor to allow proper cleanup of derived schedulers.
     */
    virtual ~Scheduler() = default;
};
