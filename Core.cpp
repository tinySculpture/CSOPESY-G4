#include "Core.h"
#include "Process.h"
#include "GlobalScheduler.h"

// Constructor: initializes core with given id
Core::Core(int id) : cid(id) {}

// Destructor: ensures the worker thread is stopped before destruction
Core::~Core() {
    stop(); // Ensure thread is stopped before destruction
}

// Starts the worker thread for this core
void Core::start() {
    running = true;
    workerThread = std::thread(&Core::run, this);
}

// Stops the worker thread safely
void Core::stop() {
    running = false;
    cv.notify_one();                // Wake thread to exit if waiting
    if (workerThread.joinable())
        workerThread.join();        // Wait for thread to finish
}

// Assigns a process to this core with a delay per execution
void Core::assignProcess(std::shared_ptr<Process> process, int delay) {
    std::lock_guard<std::mutex> lock(mtx);
    currentProcess = process;
    delayPerExec = delay;
    free = false;
    runTicks = 0;
    cv.notify_one();                // Wake thread for new assignment

    process->setCoreID(cid);        // Set the core ID for the process
}

// Returns the currently assigned process (thread-safe)
std::shared_ptr<Process> Core::getCurrentProcess() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentProcess;
}

// Preempts the current process and returns it if not finished
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

// Clears the current process assignment
void Core::clearProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    currentProcess->setCoreID(-1);
    currentProcess.reset();
    free = true;
    runTicks = 0;
}

// Checks if the core is free (not running a process)
bool Core::isFree() const {
    return free;
}

// Returns the core's ID
int Core::getId() const {
    return cid;
}

// Returns the number of ticks the current process has run
int Core::getRunTime() const {
    std::lock_guard<std::mutex> lock(mtx);
    return runTicks;
}

// Resets the run time counter for the current process
void Core::resetRunTime() {
    std::lock_guard<std::mutex> lock(mtx);
    runTicks = 0;
}

// Main worker thread loop for the core
void Core::run() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until either the core is stopped or a tick is ready
        cv.wait(lock, [&]() { 
            return !running || tickReady; 
        });

        if (!running) break; // Exit if core is stopped

        tickReady = false; 
        auto proc = currentProcess;

        proc->executeInstruction(delayPerExec); // Execute one instruction

        if (proc->isTerminated()) { // If process is terminated, clean up
            proc->setCoreID(-1);
            proc.reset();
            free = true;
            runTicks = 0;
            continue;
        }

        ++runTicks; // Increment run tick count
    }
}

// Called externally to signal a tick (time slice) to the core
void Core::tick()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (currentProcess)          
    {
        tickReady = true;
        cv.notify_one(); // Wake up the worker thread for the next tick
    }
}
