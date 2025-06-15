#include "FCFSScheduler.h"
#include <iostream>

static std::mutex coutMutex; // Mutex for console output to avoid interleaving

FCFSScheduler::FCFSScheduler(int numCores)
    : numCores(numCores),
    coreFree(numCores, true),        // now size == numCores
    coreProcesses(numCores),              // default-construct numCores Process slots
    coreMutexes(numCores),              // one mutex per core
    coreCVs(numCores)               // one CV   per core
{}

void FCFSScheduler::start() {
    // launch worker threads
    for (int c = 0; c < numCores; ++c)
        workers.emplace_back(&FCFSScheduler::workerLoop, this, c);

    // launch scheduler thread
    schedulerThread = std::thread(&FCFSScheduler::schedulerLoop, this);
}

void FCFSScheduler::stop() {
    // Wait until all queued tasks are dispatched and all cores have finished
    {
        std::unique_lock<std::mutex> lk(queueMutex);
        cvQueue.wait(lk, [this] {
            return readyQueue.empty() && allCoresFree();
            });
    }
    // Now signal shutdown
    {
        std::lock_guard<std::mutex> lk(queueMutex);
        shutdownFlag = true;
    }
    cvQueue.notify_all();
    for (auto& cv : coreCVs) cv.notify_all();

    // Join threads
    if (schedulerThread.joinable()) schedulerThread.join();
    for (auto& w : workers) if (w.joinable()) w.join();
}

void FCFSScheduler::addProcess(const Process* process) {  
    {  
        std::lock_guard<std::mutex> lk(queueMutex);  
        readyQueue.push(*process);
    } 
    cvQueue.notify_one();
}

bool FCFSScheduler::hasFreeCore() {
    for (bool free : coreFree) {
        if (free) return true;
    }
    return false;
}

bool FCFSScheduler::allCoresFree() {
    for (bool free : coreFree) {
        if (!free) return false;
    }
		return true;
}

void FCFSScheduler::schedulerLoop() {
    std::unique_lock<std::mutex> lk(queueMutex);
    while (!shutdownFlag) {
        // Wait for a new process or shutdown
        cvQueue.wait(lk, [this] {
            return shutdownFlag
                || (!readyQueue.empty() && hasFreeCore());
			});
        if (shutdownFlag) break;

        // Try dispatching as long as we have processes and free cores
        while (!readyQueue.empty()) {
            bool dispatched = false;
            {
				std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "Attempting to dispatch process..." << std::endl;
            }
            // Scan cores for the first free one
            for (int c = 0; c < numCores; ++c) {
                if (coreFree[c]) {
                    // Claim the core
                    coreFree[c] = false;
                    Process p = readyQueue.front();
                    readyQueue.pop();

                    // Push into this core’s queue
                    {
                        std::lock_guard<std::mutex> clk(coreMutexes[c]);
                        coreProcesses[c] = p;
                    }
                    coreCVs[c].notify_one();
                    dispatched = true;
                    break;
                }
            }
            if (!dispatched) { 
                {
					std::lock_guard<std::mutex> coutLock(coutMutex);
                    std::cout << "No cores free" << std::endl;
                }
                break; 
            }  // no cores free right now
        }
    }
}

void FCFSScheduler::workerLoop(int coreId) {
    while (true) {
        std::unique_lock<std::mutex> lk(coreMutexes[coreId]);
        coreCVs[coreId].wait(lk, [this, coreId] {
            return shutdownFlag || !coreProcesses[coreId].isComplete();
            });
        if (shutdownFlag && coreProcesses[coreId].isComplete())
            return;
		auto &proc = coreProcesses[coreId];

        // Execute to completion
        while (!proc.isComplete()) {
            proc.executeInstruction();
            {
				std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "[Core " << coreId << "] step "
                    << proc.getTotalInstructions()-proc.getRemainingInstruction() << "/"
                    << proc.getTotalInstructions() << "\n";
            }
        }
        {
			std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "[Core " << coreId << "] Process "
                << proc.getName() << " completed." << std::endl;
        }

        // Mark core free and wake scheduler
        {
            std::lock_guard<std::mutex> lk(queueMutex);
            coreFree[coreId] = true;
        }
        cvQueue.notify_one();
    }
}
