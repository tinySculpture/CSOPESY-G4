#pragma once  
#include "Scheduler.h"  
#include "Process.h"  

#include <queue>  
#include <vector>  
#include <thread>  
#include <mutex>  
#include <condition_variable>  

class FCFSScheduler : public Scheduler {  
public:  
    FCFSScheduler(int numCores);  
    void addProcess(std::shared_ptr<Process> process) override;  

    void start();  
    void stop();

    std::vector<std::shared_ptr<Process>> getReadyQueue() const {  
        return readyQueue;  
	}

    std::vector<std::shared_ptr<Process>> getFinishedQueue() const {  
        return finishedQueue;  
    }

private:  
    // Threads  
    void schedulerLoop();  
    void workerLoop(int coreId);  
    bool hasFreeCore();  
    bool allCoresFree();  

    int numCores;  

    // Global ready queue  
    std::vector<std::shared_ptr<Process>> readyQueue;  
    std::mutex readyQueueMutex;  
    std::condition_variable cvReadyQueue;  
    bool shutdownFlag = false;  

	// Global Finished queue
	std::vector<std::shared_ptr<Process>> finishedQueue;
	std::mutex finishedQueueMutex;
	std::condition_variable cvFinishedQueue;

    // Per-core structures  
    std::vector<bool> coreFree;  
    std::vector<std::shared_ptr<Process>> coreProcesses;  
    std::vector<std::mutex> coreMutexes;  
    std::vector<std::condition_variable> coreCVs;  

    // Threads  
    std::vector<std::thread> workers;  
    std::thread schedulerThread;  
};
