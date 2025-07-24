#include <queue>

#include "FCFSScheduler.h"

FCFSScheduler::FCFSScheduler(const SystemConfig& config)
    : numCores(config.numCPU), delaysPerExec(config.delaysPerExec) {
}

FCFSScheduler::~FCFSScheduler() {
    stop();     // Ensure clean shutdown
}

void FCFSScheduler::start() {
    shutdownFlag = false;

    // Launch core worker threads
    for (int i = 0; i < numCores; ++i) {
        auto core = std::make_unique<Core>(i);
        core->start();
        cores.emplace_back(std::move(core));
    }

    // Launch scheduler dispatcher thread
    schedulerThread = std::thread(&FCFSScheduler::schedulerLoop, this);
}

void FCFSScheduler::stop() {
    // Signal shutdown and wake scheduler
    shutdownFlag = true;
    cvReadyQueue.notify_all();

    if (schedulerThread.joinable())
        schedulerThread.join();

    // Stop and destroy cores
    for (auto& core : cores)
        core->stop();

    cores.clear();
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        // Enqueue process for scheduling
        std::lock_guard<std::mutex> lock(readyQueueMutex);
        readyQueue.push_back(process);
    }
    cvReadyQueue.notify_one();

    {
        // Record in master list of processes
        std::lock_guard<std::mutex> lock(allProcessesMutex);
        allProcesses.push_back(process);
    }
}

void FCFSScheduler::schedulerLoop() {
    while (!shutdownFlag) {
        {
            std::unique_lock<std::mutex> lock(readyQueueMutex);
            cvReadyQueue.wait_for(
                lock, TICK_PERIOD,
                [this]() {
                    return shutdownFlag.load();
                }
            );
        }

        if (shutdownFlag) break;

        // Iterate over cores for dispatch and preemption
        for (auto& core : cores) {
            auto process = core->getCurrentProcess();

            // Check for finished or quantum expiry
            if (process) {
                if (process->getRemainingInstruction() == 0) {
                    core->clearProcess();
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(readyQueueMutex);
            for (auto& up : cores) {
                if (!up->isFree() || readyQueue.empty()) continue;

                auto next = readyQueue.front();
                readyQueue.erase(readyQueue.begin());
                next->setState(ProcessState::Running);
                up->assignProcess(next, delaysPerExec, 0);
            }
        }

        for (auto& core : cores) {
            core->tick(); // Increment run time for each core
        }
    }
}

std::vector<std::shared_ptr<Process>> FCFSScheduler::getAllProcesses() const {
    std::lock_guard<std::mutex> lock(allProcessesMutex);
    return allProcesses;
}

bool FCFSScheduler::allCoresFree() {
    for (const auto& core : cores) {
        if (!core->isFree()) return false;
    }
    return true;
}

bool FCFSScheduler::noProcessFinished() {
    std::lock_guard<std::mutex> lock(allProcessesMutex);
    for (const auto& p : allProcesses) {
        if (p->getRemainingInstruction() == 0) return false;
    }
    return true;
}

std::vector<Core*> FCFSScheduler::getCores() const {
    std::vector<Core*> list;
    list.reserve(cores.size());
    for (const auto& up : cores) {
        list.push_back(up.get());
    }
    return list;
}

int FCFSScheduler::getNumCPUCycles() const {
    return numCPUCycles;
}