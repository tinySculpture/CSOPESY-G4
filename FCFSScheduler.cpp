#include <queue>

#include "FCFSScheduler.h"

FCFSScheduler::FCFSScheduler(const SystemConfig& config)
    : numCores(config.numCPU), delaysPerExec(config.delaysPerExec) {
}

FCFSScheduler::~FCFSScheduler() {
    stop();
}

void FCFSScheduler::start() {
    shutdownFlag = false;

    for (int i = 0; i < numCores; ++i) {
        auto core = std::make_unique<Core>(i);
        core->start();
        cores.emplace_back(std::move(core));
    }

    schedulerThread = std::thread(&FCFSScheduler::schedulerLoop, this);
}

void FCFSScheduler::stop() {
    shutdownFlag = true;
    cvReadyQueue.notify_all();

    if (schedulerThread.joinable())
        schedulerThread.join();

    for (auto& core : cores)
        core->stop();

    cores.clear();
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
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

void FCFSScheduler::schedulerLoop() {
    while (!shutdownFlag) {
        std::unique_lock<std::mutex> lock(readyQueueMutex);
        cvReadyQueue.wait(lock, [&]() {
            return shutdownFlag || !readyQueue.empty();
            });

        if (shutdownFlag) break;

        for (auto& core : cores) {
            if (core->isFree() && !readyQueue.empty()) {
                auto process = readyQueue.front();
                readyQueue.erase(readyQueue.begin());

                process->setState(ProcessState::Ready);
                process->setCoreID(core->getId());

                core->assignProcess(process, delaysPerExec);
            }
        }

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