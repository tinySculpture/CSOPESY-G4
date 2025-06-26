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

class FCFSScheduler : public Scheduler {
public:
    FCFSScheduler(const SystemConfig& config);
    ~FCFSScheduler();

    void start() override;
    void stop() override;
    void addProcess(std::shared_ptr<Process> process) override;

    std::vector<std::shared_ptr<Process>> getAllProcesses() const override;
    bool allCoresFree() override;
    bool noProcessFinished() override;

private:
    void schedulerLoop();

    std::vector<std::unique_ptr<Core>> cores;
    std::thread schedulerThread;
    std::atomic<bool> shutdownFlag = false;

    std::vector<std::shared_ptr<Process>> allProcesses;
    mutable std::mutex allProcessesMutex;

    std::vector<std::shared_ptr<Process>> readyQueue;
    mutable std::mutex readyQueueMutex;
    std::condition_variable cvReadyQueue;

    int numCores;
    unsigned long delaysPerExec;
};