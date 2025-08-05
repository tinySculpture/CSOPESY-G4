#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Core.h"
#include "SystemConfig.h"
#include "Scheduler.h"
#include "Process.h"
#include "Globals.h"
#include "ConsoleUtil.h"

/**
 * @class RRScheduler
 * @brief Round-Robin process scheduler using multiple CPU cores.
 *
 * Maintains a FIFO queue of ready processes, dispatches them to available cores,
 * and enforces preemption when a process runs longer than the configured quantum.
 */
class RRScheduler : public Scheduler {
public:
    /**
     * @brief Construct a Round-Robin scheduler with system configuration.
     *
     * @param config Provides numCPU, delaysPerExec, and quantumCycles.
     */
    RRScheduler();



    /**
     * @brief Destructor stops the scheduler and all core threads.
     */
    ~RRScheduler();



    /**
     * @brief Start the scheduler and worker cores.
     */
    void start();



    /**
     * @brief Stop scheduling, join threads, and clear resources.
     */
    void stop();



    /**
     * @brief Add a process to the ready queue for Round-Robin dispatch.
     *
     * @param process Shared pointer to the Process to schedule.
     */
    void addProcess(std::shared_ptr<Process> process);



    /**
     * @brief Retrieve all processes ever scheduled.
     *
     * @return Vector of shared pointers to all added processes.
     */
    std::vector<std::shared_ptr<Process>> getAllProcesses() const;



    /**
     * @brief Check if all CPU cores are currently idle.
     *
     * @return true if no core is executing a process.
     */
    bool allCoresFree();



    /**
     * @brief Check if no process has finished execution yet.
     *
     * @return true if all processes still have remaining instructions.
     */
    bool noProcessFinished();

    std::vector<Core*> getCores() const;

    int getNumCPUCycles() const;

private:
    /**
     * @brief Main loop of the RR scheduler.
     *
     * Waits for processes in the ready queue, dispatches them to cores,
     * and preempts based on quantum. Runs until stop() is called.
     */
    void schedulerLoop();



    /**
     * @brief Re-enqueue a preempted process at the back of the ready queue.
     *
     * @param process Shared pointer to the preempted Process.
     */
    void addToQueue(std::shared_ptr<Process> process);

    std::vector<std::unique_ptr<Core>> cores;               // Owned CPU Cores
    std::vector<std::shared_ptr<Process>> readyQueue;       // RR ready queue
    std::vector<std::shared_ptr<Process>> allProcesses;     // All processes added

    std::thread schedulerThread;                            // Thread for schedulerLoop()
    std::atomic<bool> shutdownFlag{ false };                // Flag to stop schedulerLoop()

    mutable std::mutex readyQueueMutex;         // Guards ready queue
    mutable std::mutex allProcessesMutex;       // Guards allProcesses
    std::condition_variable cvReadyQueue;       // Notifies new arrivals or shutdown

    int numCores;           // Number of CPU cores
    int delaysPerExec;      // Delay ticks between instructions
    int quantumCycles;      // Time slice length in ticks before preemption

	int numCPUCycles = 0; // Total CPU cycles executed
};