#include <chrono>
#include <iostream>
#include <queue>

#include "RRScheduler.h"

RRScheduler::RRScheduler(const SystemConfig& config)
    : numCores(config.numCPU), delaysPerExec(config.delaysPerExec),
    quantumCycles(config.quantumCycles) {
}

RRScheduler::~RRScheduler() {
    stop(); // Ensure threads are cleaned up
}

void RRScheduler::start() {
    shutdownFlag = false;

    // Launch core worker threads
    for (int i = 0; i < numCores; ++i) {
        auto core = std::make_unique<Core>(i);
        core->start();
        cores.emplace_back(std::move(core));
    }

    // Launch scheduler dispatcher thread
    schedulerThread = std::thread(&RRScheduler::schedulerLoop, this);
}

void RRScheduler::stop() {
    // signal shutdown and wake dispatcher
    shutdownFlag = true;
    cvReadyQueue.notify_all();

    if (schedulerThread.joinable())
        schedulerThread.join();

    // Stop and destroy cores
    for (auto& core : cores)
        core->stop();

    cores.clear();
}

void RRScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::mutex> lock(readyQueueMutex);
        readyQueue.push_back(process);
    }
    cvReadyQueue.notify_one();

    {
        std::lock_guard<std::mutex> lock(allProcessesMutex);
        allProcesses.push_back(process);
    }
}

void RRScheduler::schedulerLoop() {
    while (!shutdownFlag) {
        {
            std::unique_lock<std::mutex> lock(readyQueueMutex);
            cvReadyQueue.wait(lock, [&]() {
                return shutdownFlag || !readyQueue.empty();
                });
        }

        // Iterate over cores for dispatch and preemption
        for (auto& core : cores) {
            auto process = core->getCurrentProcess();

            // Check for finished or quantum expiry
            if (process) {
                if (process->isFinished()) {
                    core->clearProcess();
                    continue;
                }

                if (core->getRunTime() >= quantumCycles) {
                    auto preempted = core->preemptProcess();
                    if (preempted)
                        addToQueue(preempted);
                }
            }

            // Assign next process if core is free
            if (core->isFree()) {
                std::shared_ptr<Process> next = nullptr;

                {
                    std::lock_guard<std::mutex> lock(readyQueueMutex);
                    if (!readyQueue.empty()) {
                        next = readyQueue.front();
                        readyQueue.erase(readyQueue.begin());
                    }
                }

                if (next) {
                    next->setState(ProcessState::Ready);
                    core->assignProcess(next, delaysPerExec);
                }
            }
        }

        // Throttle dispatch loop to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void RRScheduler::addToQueue(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(readyQueueMutex);
    readyQueue.push_back(process);
}


std::vector<std::shared_ptr<Process>> RRScheduler::getAllProcesses() const {
    std::lock_guard<std::mutex> lock(allProcessesMutex);
    return allProcesses;
}

bool RRScheduler::allCoresFree() {
    for (const auto& core : cores) {
        if (!core->isFree()) return false;
    }
    return true;
}

bool RRScheduler::noProcessFinished() {
    std::lock_guard<std::mutex> lock(allProcessesMutex);
    for (const auto& p : allProcesses) {
        if (p->getRemainingInstruction() == 0) return false;
    }
    return true;
}
