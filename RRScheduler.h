#pragma once

#include "Scheduler.h"
#include "Core.h"
#include "SystemConfig.h"

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class RRScheduler : public Scheduler {
public:
    RRScheduler(const SystemConfig& config);
    ~RRScheduler();

    void start() override;
    void stop() override;
    void addProcess(std::shared_ptr<Process> process) override;

    std::vector<std::shared_ptr<Process>> getAllProcesses() const override;
    bool allCoresFree() override;
    bool noProcessFinished() override;

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