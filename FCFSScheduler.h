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
    FCFSScheduler();
	~FCFSScheduler() = default;

    void start();  
    void stop();

    void addProcess(std::shared_ptr<Process> process);

    std::vector<std::shared_ptr<Process>> getAllProcesses() const;

    bool allCoresFree();
	bool noProcessFinished();


private:  
    // Threads 
    void schedulerLoop();  
    void workerLoop(int coreId);  

    std::atomic<uint64_t> cpuTicks;

    bool hasFreeCore();    

    int numCores;  

	// All processes
	std::vector<std::shared_ptr<Process>> allProcesses;
	std::mutex allProcessesMutex;
	std::condition_variable cvAllProcesses;

    // Global ready queue  
    std::vector<std::shared_ptr<Process>> readyQueue;  
    std::mutex readyQueueMutex;  
    std::condition_variable cvReadyQueue;  
    bool shutdownFlag = false;

    // Per-core structures  
    std::vector<bool> coreFree;  
    std::vector<std::shared_ptr<Process>> coreProcesses;  
    std::vector<std::mutex> coreMutexes;  
    std::vector<std::condition_variable> coreCVs;  

    // Threads  
    std::vector<std::thread> workers;  
    std::thread schedulerThread;  
};
