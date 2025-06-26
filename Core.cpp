#include <chrono>

#include "Core.h"

Core::Core(int id) : cid(id) {}

Core::~Core() {
    stop();
}

void Core::start() {
    running = true;
    workerThread = std::thread(&Core::run, this);
}

void Core::stop() {
    running = false;
    cv.notify_one();
    if (workerThread.joinable())
        workerThread.join();
}

void Core::assignProcess(std::shared_ptr<Process> process, int delay) {
    std::lock_guard<std::mutex> lock(mtx);
    currentProcess = process;
    delayPerExec = delay;
    free = false;
    runTicks = 0;
    cv.notify_one();

    process->setCoreID(cid);
}

std::shared_ptr<Process> Core::getCurrentProcess() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentProcess;
}

std::shared_ptr<Process> Core::preemptProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    if (currentProcess && !currentProcess->isFinished()) {
        currentProcess->setState(ProcessState::Ready);
        currentProcess->setCoreID(-1);
        auto p = currentProcess;
        currentProcess.reset();
        free = true;
        runTicks = 0;
        return p;
    }
    return nullptr;
}

void Core::clearProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    currentProcess.reset();
    free = true;
    runTicks = 0;
}


bool Core::isFree() const {
    return free;
}

int Core::getId() const {
    return cid;
}

int Core::getRunTime() const {
    std::lock_guard<std::mutex> lock(mtx);
    return runTicks;
}

void Core::resetRunTime() {
    std::lock_guard<std::mutex> lock(mtx);
    runTicks = 0;
}

void Core::run() {
    while (running) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !running || currentProcess != nullptr; });

        if (!running) break;

        if (currentProcess) {
            currentProcess->setCoreID(cid);

            while (currentProcess && !currentProcess->isFinished()) {
                currentProcess->executeInstruction(delayPerExec);
                ++runTicks;

                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                lock.lock();
            }

            if (currentProcess && currentProcess->isFinished()) {
                currentProcess->setState(ProcessState::Finished);
                currentProcess->setCoreID(-1);
                currentProcess.reset();
            }

            free = true;
            runTicks = 0;
        }
    }
}