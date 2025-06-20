#include "FCFSScheduler.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <chrono>
#include <windows.h>

static std::mutex coutMutex; // Mutex for console output to avoid interleaving

FCFSScheduler::FCFSScheduler(int numCores)
    : numCores(numCores),
    coreFree(numCores, true),        // now size == numCores
    coreProcesses(numCores, nullptr),              // default-construct numCores Process slots
    coreMutexes(numCores),              // one mutex per core
    coreCVs(numCores),               // one CV   per core
	shutdownFlag(false),
	cpuTicks(0)
{}

uint64_t FCFSScheduler::getCpuTicks() { 
    return cpuTicks.load(std::memory_order_relaxed); 
}

void FCFSScheduler::start() {
    // launch worker threads
    for (int c = 0; c < numCores; ++c)
        workers.emplace_back(&FCFSScheduler::workerLoop, this, c);

    // launch scheduler thread
    schedulerThread = std::thread(&FCFSScheduler::schedulerLoop, this);
}

void FCFSScheduler::stop() {
    // Now signal shutdown
    {
        std::lock_guard<std::mutex> lk(readyQueueMutex);
        shutdownFlag = true;
    }
    cvReadyQueue.notify_all();
    for (auto& cv : coreCVs) cv.notify_all();

    // Detach threads
    if (schedulerThread.joinable()) schedulerThread.detach();
    for (auto& w : workers) if (w.joinable()) w.detach();
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
    std::lock_guard<std::mutex> lk(allProcessesMutex);
    for (const auto& p : allProcesses) {
        if (p->getRemainingInstruction() == 0) return false;
    }
    return true;
}

void FCFSScheduler::schedulerLoop() {
    std::unique_lock<std::mutex> lk(readyQueueMutex);
    while (!shutdownFlag) {
        // Wait for a new process or shutdown
        cvReadyQueue.wait(lk, [this] {
            return shutdownFlag
                || (!readyQueue.empty() && hasFreeCore());
			});
        if (shutdownFlag) break;

        // Try dispatching as long as we have processes and free cores
        while (!readyQueue.empty()) {
            bool dispatched = false;
            /*{
				std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "Attempting to dispatch process..." << std::endl;
            }*/
            // Scan cores for the first free one
            for (int c = 0; c < numCores; ++c) {
                //std::cout << "Searching for free cores..." << std::endl;
                if (coreFree[c]) {
                    //std::cout << "Found free core (core " << c << ")..." << std::endl;
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
                    dispatched = true;
                    break;
                }
            }
            if (!dispatched) { 
				readyQueue.front()->setCoreID(-1); // No core available, set core ID to -1
                break; 
            }  // no cores free right now
        }
    }
}

void FCFSScheduler::workerLoop(int coreId) {
    while (true) {
		std::shared_ptr<Process> proc;

        {
            std::unique_lock<std::mutex> lk(coreMutexes[coreId]);
            coreCVs[coreId].wait(lk, [this, coreId] {
                return shutdownFlag || coreProcesses[coreId] != nullptr;
                });

            if (shutdownFlag && !coreProcesses[coreId])
                return;

            proc = coreProcesses[coreId];
            //std::cout << "To be executed" << std::endl;
        }

        // Execute to completion
        int total = proc->getTotalInstructions();
        while (proc->getRemainingInstruction() > 0) {
            proc->executeInstruction();
            
            cpuTicks.fetch_add(1, std::memory_order_relaxed); // Increment CPU ticks
			Sleep(100); // Simulate some processing time
        }

        // TEMPORARY for print function
		proc->logInstructionsToFile();

        {
            std::lock_guard<std::mutex> lk(coreMutexes[coreId]);
            coreProcesses[coreId].reset();
        }
		coreCVs[coreId].notify_one(); // Notify scheduler that core is free

        // Mark core free and wake scheduler
        {
            std::lock_guard<std::mutex> lk(readyQueueMutex);
            coreFree[coreId] = true;
        }
        cvReadyQueue.notify_one();
    }
}
