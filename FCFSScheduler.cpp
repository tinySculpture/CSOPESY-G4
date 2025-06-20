#include "FCFSScheduler.h"
#include "GlobalScheduler.h"
#include "ConsoleUtil.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <chrono>
#include <windows.h>

static std::mutex coutMutex; // Mutex for console output to avoid interleaving

FCFSScheduler::FCFSScheduler()
    : numCores(2),
    coreFree(numCores, true),        // now size == numCores
    coreProcesses(numCores, nullptr),              // default-construct numCores Process slots
    coreMutexes(numCores),              // one mutex per core
    coreCVs(numCores),               // one CV   per core
    shutdownFlag(false),
    cpuTicks(0)
{}



void FCFSScheduler::start() {
   // launch worker threads
    for (int c = 0; c < numCores; ++c)
        workers.emplace_back(&FCFSScheduler::workerLoop, this, c);

    // launch scheduler thread
    schedulerThread = std::thread(&FCFSScheduler::schedulerLoop, this);
}



void FCFSScheduler::stop() {
    // Detach threads
    schedulerThread.detach();
    for (auto& w : workers) w.detach();
}



void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    {  
        std::lock_guard<std::mutex> lk(readyQueueMutex);  
        readyQueue.push_back(process);
    } 
    cvReadyQueue.notify_one();

    {
		std::lock_guard<std::mutex> lk(allProcessesMutex);
        allProcesses.push_back(process);
    }
    cvAllProcesses.notify_one();
}



void FCFSScheduler::schedulerLoop() {
    while (!shutdownFlag) {
        if (shutdownFlag) break;

        // Try dispatching as long as we have processes and free cores
        if (!readyQueue.empty() && hasFreeCore()) {
            // Scan cores for the first free one
            for (int c = 0; c < numCores; ++c) {
                if (coreFree[c]) {
                    // Claim the core
                    coreFree[c] = false;
                    std::shared_ptr<Process> p = readyQueue.front();
					readyQueue.erase(readyQueue.begin()); // Remove from readyQueue

                    // Push into this core’s queue
                    {
						std::lock_guard<std::mutex> clk(coreMutexes[c]);
                        coreProcesses[c] = std::move(p);
						coreProcesses[c]->setCoreID(c);
                    }
					coreCVs[c].notify_one(); // Wake the worker thread for this core
                    break;
                }
            }
        }
    }
}



void FCFSScheduler::workerLoop(int coreId) {
    while (true) {
        if (shutdownFlag) break;

        std::shared_ptr<Process> proc;

        if (coreProcesses[coreId] != nullptr) {
			proc = coreProcesses[coreId];

            // Execute to completion
            while (proc->getRemainingInstruction() > 0) {
                proc->executeInstruction();
                Sleep(100); // Simulate some processing time
            }
            {
                std::lock_guard<std::mutex> lk(coreMutexes[coreId]);
                coreProcesses[coreId].reset();
            }
            coreCVs[coreId].notify_one(); // Notify scheduler that core is free

            // Mark core free and wake scheduler
            coreFree[coreId] = true;
        }
    }
}


// Utility functions
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



bool FCFSScheduler::noProcessFinished() {
    for (const auto& p : allProcesses) {
        if (p->getRemainingInstruction() == 0) return false;
    }
    return true;
}



std::vector<std::shared_ptr<Process>> FCFSScheduler::getAllProcesses() const {
    return allProcesses;
}