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
    void addProcess(const Process* process);

    void start();
    void stop();

private:
    // Threads
    void schedulerLoop();
    void workerLoop(int coreId);
	bool hasFreeCore();
    bool allCoresFree();

    int numCores;

    // Global ready queue
    std::queue<Process>           readyQueue;
    std::mutex                    queueMutex;
    std::condition_variable       cvQueue;
    bool                           shutdownFlag = false;

    // Per-core structures
    std::vector<bool>                            coreFree;
    std::vector<Process>                         coreProcesses;
    std::vector<std::mutex>                      coreMutexes;
    std::vector<std::condition_variable>         coreCVs;

    // Threads
    std::vector<std::thread>      workers;
    std::thread                   schedulerThread;
};
