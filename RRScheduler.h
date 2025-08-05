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

class RRScheduler : public Scheduler {
public:
    RRScheduler(const SystemConfig& config);
    ~RRScheduler();

    void start();
    void stop();

    void addProcess(std::shared_ptr<Process> process);
    std::vector<std::shared_ptr<Process>> getAllProcesses() const;

    bool allCoresFree();
    bool noProcessFinished();

    std::vector<Core*> getCores() const;

private:
    void schedulerLoop();
    void addToQueue(std::shared_ptr<Process> process);

    std::vector<std::unique_ptr<Core>> cores;               
    std::vector<std::shared_ptr<Process>> readyQueue;       
    std::vector<std::shared_ptr<Process>> allProcesses;     

    std::thread schedulerThread;                            
    std::atomic<bool> shutdownFlag{ false };                

    mutable std::mutex readyQueueMutex;         
    mutable std::mutex allProcessesMutex;       
    std::condition_variable cvReadyQueue;       

    int numCores;           
    int delaysPerExec;     
    int quantumCycles;     
};