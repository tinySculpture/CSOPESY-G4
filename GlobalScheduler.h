#pragma once  

#include <memory>
#include <unordered_map>

#include "SystemConfig.h"
#include "FCFSScheduler.h"
#include "RRScheduler.h"
#include "Scheduler.h"  

/**
 * @class GlobalScheduler
 * @brief Manages scheduler instances and delegates scheduling operations.
 *
 * Implements the Singleton pattern to ensure a single global entry point.
 * Supports startup/shutdown, process submission, querying, and dynamic
 * switching between FCFS and Round-Robin schedulers.
 */
class GlobalScheduler {
public:
    /**
     * @brief Initialize the singleton with system configuration.
     *
     * Must be called before getInstance(). Creates FCFS and RR schedulers,
     * selects the active one based on config.scheduler, and stores it.
     *
     * @param config Configuration specifying scheduler type and CPU count.
     */
    static void initialize(const SystemConfig& config);



    /**
     * @brief Retrieve the singleton instance.
     *
     * @return Pointer to the initialized GlobalScheduler, or nullptr if not yet initialized.
     */
    static GlobalScheduler* getInstance();



    /**
     * @brief Destroy the singleton, stopping any active scheduler.
     */
    static void destroy();



    /**
     * @brief Start the selected scheduler.
     */
    void start();



    /**
     * @brief Stop the selected scheduler.
     */
    void stop();



    /**
     * @brief Submit a process for scheduling.
     *
     * Delegates to the active Scheduler implementation.
     *
     * @param process Shared pointer to the Process to schedule.
     */
    void addProcess(std::shared_ptr<Process> process);



    /**
     * @brief Retrieve all processes known to the active scheduler.
     *
     * @return Vector of shared pointers to all scheduled processes.
     */
    std::vector<std::shared_ptr<Process>> getAllProcesses() const;



    /**
     * @brief Dynamically switch the active scheduler by name.
     *
     * Starts the newly selected scheduler immediately.
     * If the name is invalid, prints an error and retains the current one.
     *
     * @param name Lowercase string identifier: "fcfs" or "rr".
     */
    void setCurrentScheduler(std::string name);




    /**
     * @brief Check if all cores across the active scheduler are free.
     *
     * @return true if no process is running.
     */
    bool allCoresFree() const;

    /**
     * @brief Check if no process has finished execution yet.
     *
     * @return true if all processes still have remaining instructions.
     */
    bool noProcessFinished() const;

    /**
     * @brief Query whether the scheduler is currently running.
     *
     * @return true if start() was called without a matching stop().
     */
    bool isRunning();

private:
    /**
     * @brief Private constructor to enforce Singleton.
     *
     * @param config System configuration used to instantiate schedulers.
     */
    GlobalScheduler(const SystemConfig& config);

    inline static GlobalScheduler* sharedInstance = nullptr;    // Singleton instance
    bool running = false;                                       // Scheduler running state

    std::unordered_map<std::string, std::shared_ptr<Scheduler>> schedulers;     // Available schedulers
    std::shared_ptr<Scheduler> currentScheduler;                                // Active Scheduler
};