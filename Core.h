#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "Process.h"

class Core {
public:
    Core(int id);
    ~Core();

    void start();
    void stop();

    void assignProcess(std::shared_ptr<Process> process, int delayPerExec);
    std::shared_ptr<Process> getCurrentProcess() const;
    std::shared_ptr<Process> preemptProcess();
    void clearProcess();

    bool isFree() const;
    int getId() const;

    int getRunTime() const;
    void resetRunTime();

private:
    void run();

    int cid;
    std::atomic<bool> running = false;
    std::atomic<bool> free = true;

    mutable std::mutex mtx;
    std::condition_variable cv;
    std::thread workerThread;

    int delayPerExec = 0;
    int runTicks = 0;

    std::shared_ptr<Process> currentProcess = nullptr;

};