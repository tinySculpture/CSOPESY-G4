#include <chrono>
#include <iostream>
#include <queue>

#include "RRScheduler.h"
#include "GlobalScheduler.h"

RRScheduler::RRScheduler(const SystemConfig& config)
    : numCores(config.numCPU), delaysPerExec(config.delaysPerExec),
    quantumCycles(config.quantumCycles) {
}

RRScheduler::~RRScheduler() {
    stop(); 
}

void RRScheduler::start() {
    shutdownFlag = false;

    for (int i = 0; i < numCores; ++i) {
        auto core = std::make_unique<Core>(i);
        core->start();
        cores.emplace_back(std::move(core));
    }

    schedulerThread = std::thread(&RRScheduler::schedulerLoop, this);
}

void RRScheduler::stop() {
    shutdownFlag = true;
    cvReadyQueue.notify_all();

    if (schedulerThread.joinable())
        schedulerThread.join();

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
            cvReadyQueue.wait_for(
                lock, TICK_PERIOD,
                [this]() {
                    return shutdownFlag.load();
                }
            );
        }

		if (shutdownFlag) break;

        for (auto& core : cores) {
            auto process = core->getCurrentProcess();

            if (process) {
                if (process->getRemainingInstruction() == 0) {
                    core->clearProcess();
                    process->setState(ProcessState::Finished);

                    MemoryManager::getInstance()->freeProcessPages(process->getPID());

                    Process::unregisterProcess(process->getPID());
                }
                else if (core->getRunTime() >= quantumCycles) {
                    auto preempted = core->preemptProcess();
                    if (preempted) {
                        addToQueue(preempted);
                    }
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(readyQueueMutex);
            for (auto& up : cores) {
                if (!up->isFree()) continue;

                std::shared_ptr<Process> nextProcess = nullptr;

                for (auto it = readyQueue.begin(); it != readyQueue.end();) {
                    auto proc = *it;

                    if (proc->getState() == ProcessState::Blocked || proc->isTerminated()) {
                        it = readyQueue.erase(it);  
                        continue;
                    }

                    if (proc->getState() == ProcessState::Ready) {
                        nextProcess = proc;
                        it = readyQueue.erase(it);
                        break;
                    }

                    ++it;
                }

                if (nextProcess) {
                    nextProcess->setState(ProcessState::Running);
                    up->assignProcess(nextProcess, delaysPerExec);
                }
            }
        }

        auto globalScheduler = GlobalScheduler::getInstance();

        for (auto& core : cores) {
            if (core->getCurrentProcess()) {
                globalScheduler->incrementActiveTicks();
            } else {
                globalScheduler->incrementIdleTicks();
            }
            core->tick();
        }


    }
}

void RRScheduler::addToQueue(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(readyQueueMutex);
    if (process->getState() == ProcessState::Blocked) {
        return;
    }
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