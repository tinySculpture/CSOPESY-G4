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
        std::unique_lock<std::mutex> lock(readyQueueMutex);

        // Wait until there is work or shutdown requested
        cvReadyQueue.wait(lock, [&]() {
            return shutdownFlag || !readyQueue.empty();
            });

        if (shutdownFlag) break;

        // Dispatch ready processes to free cores
        for (auto& core : cores) {
            if (core->isFree() && !readyQueue.empty()) {
                auto process = readyQueue.front();
                readyQueue.erase(readyQueue.begin());

                process->setState(ProcessState::Ready);
                process->setCoreID(core->getId());

                // Assign process to core
                core->assignProcess(process, delaysPerExec);
            }
        }

        // Small sleep to reduce busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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