#include <queue>

#include "FCFSScheduler.h"
#include "GlobalScheduler.h"

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
            }
        }

        {
            std::lock_guard<std::mutex> lock(readyQueueMutex);
            for (auto& up : cores) {
                if (!up->isFree()) continue;

                std::shared_ptr<Process> nextProcess = nullptr;

                for (auto it = readyQueue.begin(); it != readyQueue.end(); ) {
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