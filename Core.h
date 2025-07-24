#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

class Process;

class GlobalScheduler; // Forward declaration for GlobalScheduler

/**
* @class Core
* @brief Encapsulates a simulated CPU core for running Process instances.
*
* Each Core runs in its own thread, waits for process assignments, executes
* instructions with configurable delays, and handles preemption and cleanup.
*/
class Core {
public:
    /**
     * @brief Construct a Core with a given identifier.
     *
     * @param id Numeric ID of this core (0-based).
     */
    Core(int id);



    /**
     * @brief Destructor stops the core thread cleanly.
     */
    ~Core();



    /**
     * @brief Start the core's execution thread.
     *
     * Sets the running flag and launches the worker thread calling run().
     */
    void start();



    /**
     * @brief Stop the core's execution thread.
     *
     * Clears the running flag, notifies the thread, and joins it.
     */
    void stop();



    /**
     * @brief Assign a Process to this core for execution.
     *
     * @param process   Shared pointer to the Process to run.
     * @param delayPerExec Number of ticks to delay between instructions.
     */
    void assignProcess(std::shared_ptr<Process> process, int delayPerExec, size_t memory);



    /**
     * @brief Get the currently assigned Process.
     *
     * @return Shared pointer to the current Process, or nullptr if idle.
     */
    std::shared_ptr<Process> getCurrentProcess() const;



    /**
     * @brief Preempt the current process and make the core free.
     *
     * If a process is running and not finished, reset its state to Ready
     * and detach it from this core.
     *
     * @return Shared pointer to the preempted Process, or nullptr if none.
     */
    std::shared_ptr<Process> preemptProcess();



    /**
     * @brief Clear any assigned process, marking the core as free.
     */
    void clearProcess();



    /**
     * @brief Check if the core is available for process assignment.
     *
     * @return true if no process is currently assigned.
     */
    bool isFree() const;



    /**
     * @brief Get this core's identifier.
     *
     * @return Core ID as provided at construction.
     */
    int getId() const;



    /**
     * @brief Get the number of instruction execution ticks since last reset.
     *
     * @return Run ticks count.
     */
    int getRunTime() const;



    /**
     * @brief Reset the runTicks counter to zero.
     */
    void resetRunTime();

	void tick();

private:
    /**
     * @brief Main loop executed by the worker thread.
     *
     * Waits for process assignments, executes instructions with delays,
     * and handles process completion and core state transitions.
     */
    void run();

    int cid;                                // Core identifier
    std::atomic<bool> running = false;      // Flag to control thread lifetime
    std::atomic<bool> free = true;          // True when no process is assigned

    mutable std::mutex mtx;                 // Mutex guarding shared state
    std::condition_variable cv;             // CV to notify assignment or stop
    std::thread workerThread;               // Thread running the core

    int delayPerExec = 0;                   // Delay ticks between instructions
    int runTicks = 0;                       // Count of executed instruction ticks

	bool tickReady = false;          // True if ready to execute next instruction

    std::shared_ptr<Process> currentProcess = nullptr;  // currently assigned process.

};