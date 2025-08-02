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
		// Sleep for a tick period
        std::this_thread::sleep_for(TICK_PERIOD);

		if (shutdownFlag) break;

        // Iterate over cores for dispatch and preemption
        for (auto& core : cores) {
            auto process = core->getCurrentProcess();

            // Check for finished or quantum expiry
            if (process) {
                if (process->getRemainingInstruction() == 0) {
                    core->clearProcess();
                }
                else if (core->getRunTime() >= quantumCycles && !readyQueue.empty()) {
                    // Preempt the current process if quantum expired
                    auto preempted = core->preemptProcess();
                    if (preempted) {
                        {
                            std::unique_lock<std::mutex> lock(readyQueueMutex);
                            addToQueue(preempted);
                        }
                    }
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(readyQueueMutex);
            for (auto& up : cores) {
                if (!up->isFree() || readyQueue.empty()) continue;
				auto next = readyQueue.front();
                readyQueue.erase(readyQueue.begin()); // Remove from ready queue
                next->setState(ProcessState::Running);
                up->assignProcess(next, delaysPerExec);
            }
        }

        for (auto& core : cores) {
            core->tick(); // Increment run time for each core
        }

        numCPUCycles++;
    }
}

void RRScheduler::addToQueue(std::shared_ptr<Process> process) {
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

std::vector<Core*> RRScheduler::getCores() const {
    std::vector<Core*> list;
    list.reserve(cores.size());
    for (const auto& up : cores) {
        list.push_back(up.get());
    }
    return list;
}

int RRScheduler::getNumCPUCycles() const {
    return numCPUCycles;
}