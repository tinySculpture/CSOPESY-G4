#include "FCFSScheduler.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <chrono>

static std::mutex coutMutex; // Mutex for console output to avoid interleaving

FCFSScheduler::FCFSScheduler(int numCores)
    : numCores(numCores),
    coreFree(numCores, true),        // now size == numCores
    coreProcesses(numCores, nullptr),              // default-construct numCores Process slots
    coreMutexes(numCores),              // one mutex per core
    coreCVs(numCores),               // one CV   per core
	shutdownFlag(false)
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
        std::unique_lock<std::mutex> lk(readyQueueMutex);
        cvReadyQueue.wait(lk, [this] {
            return readyQueue.empty() && allCoresFree();
            });
    }
    // Now signal shutdown
    {
        std::lock_guard<std::mutex> lk(readyQueueMutex);
        shutdownFlag = true;
    }
    cvReadyQueue.notify_all();
    for (auto& cv : coreCVs) cv.notify_all();

    // Join threads
    if (schedulerThread.joinable()) schedulerThread.join();
    for (auto& w : workers) if (w.joinable()) w.join();
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    {  
        std::lock_guard<std::mutex> lk(readyQueueMutex);  
        readyQueue.push_back(process);
    } 
    cvReadyQueue.notify_one();
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
                        //std::cout << "Dispatched process " << coreProcesses[c]->getName() << " to core " << c << std::endl;
                    }
					coreCVs[c].notify_one(); // Wake the worker thread for this core
                    dispatched = true;
                    break;
                }
            }
            if (!dispatched) { 
                {
					//std::lock_guard<std::mutex> coutLock(coutMutex);
                    //std::cout << "No cores free" << std::endl;
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
            return shutdownFlag || coreProcesses[coreId] != nullptr;
            });

        if (shutdownFlag && !coreProcesses[coreId])
            return;

        auto proc = coreProcesses[coreId];
        //std::cout << "To be executed" << std::endl;

        coreProcesses[coreId].reset();

        // Execute to completion
        while (proc->getRemainingInstruction() > 0) {
            proc->executeInstruction();
            /*{
				std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "[Core " << coreId << "] step "
                    << proc->getTotalInstructions()-proc->getRemainingInstruction() << "/"
                    << proc->getTotalInstructions() << "\n";
            }*/
        }
        /*{
			std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "[Core " << coreId << "] Process "
                << proc->getName() << " completed." << std::endl;
        }*/

        // Push finished job into the finishedQueue
        {
            std::lock_guard<std::mutex> fqLock(finishedQueueMutex);
            finishedQueue.push_back(proc);
        }
        cvFinishedQueue.notify_one();

        // Mark core free and wake scheduler
        {
            std::lock_guard<std::mutex> lk(readyQueueMutex);
            coreFree[coreId] = true;
        }
        cvReadyQueue.notify_one();
    }
}
