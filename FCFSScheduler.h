#pragma once  

#include <atomic>
#include <condition_variable> 
#include <mutex> 
#include <queue>  
#include <thread> 
#include <vector>    

#include "Core.h"  
#include "SystemConfig.h"
#include "Scheduler.h"

#include "Process.h"

/**
 * @class FCFSScheduler
 * @brief FCFS-based process scheduler using multiple CPU cores.
 *
 * Processes are enqueued in arrival order and dispatched to the next free core.
 * Supports clean startup/shutdown, process addition, and status queries.
 */
class FCFSScheduler : public Scheduler {
public:
    /**
     * @brief Construct the scheduler with system configuration.
     *
     * @param config SystemConfig containing CPU count and execution delays.
     */
    FCFSScheduler(const SystemConfig& config);



    /**
     * @brief Destructor stops the scheduler and all core threads.
     */
    ~FCFSScheduler();



    /**
     * @brief Start the scheduler and worker cores.
     *
     * Launches a thread per Core and the main scheduling loop.
     */
    void start() override;



    /**
     * @brief Stop scheduling, join threads, and clear resources.
     */
    void stop() override;



    /**
     * @brief Add a process to the ready queue for future dispatch.
     *
     * @param process Shared pointer to the Process to schedule.
     */
    void addProcess(std::shared_ptr<Process> process) override;



    /**
     * @brief Retrieve all processes ever scheduled.
     *
     * @return Vector of shared pointers to all added processes.
     */
    std::vector<std::shared_ptr<Process>> getAllProcesses() const override;



    /**
     * @brief Check if all CPU cores are currently idle.
     *
     * @return true if no core is executing a process.
     */
    bool allCoresFree() override;



    /**
     * @brief Check if no process has finished execution yet.
     *
     * @return true if all processes still have remaining instructions.
     */
    bool noProcessFinished() override;

private:
    /**
     * @brief Main loop of the FCFS scheduler.
     *
     * Waits for processes in the ready queue and assigns them to free cores.
     */
    void schedulerLoop();

    std::vector<std::unique_ptr<Core>> cores;               // Owned CPU Cores
    std::thread schedulerThread;                            // Thread for schedulerLoop().
    std::atomic<bool> shutdownFlag = false;                 // Signal to terminate scheduling

    std::vector<std::shared_ptr<Process>> allProcesses;     // All processes added
    mutable std::mutex allProcessesMutex;                   // Guards all processes
        
    std::vector<std::shared_ptr<Process>> readyQueue;       // FIFO queue of ready processes
    mutable std::mutex readyQueueMutex;                     // Guards ready queue
    std::condition_variable cvReadyQueue;                   // Notifies when new process arrives or shutdown

    int numCores;                                           // Number of cores from Config
    unsigned long delaysPerExec;                            // Delay Ticks between instruction executions
};