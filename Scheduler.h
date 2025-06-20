#pragma once
#include <memory>

#include "Process.h"

// Abstract base class for different CPU scheduling algorithms
class Scheduler {
public:
	virtual void start() = 0;
    virtual void stop() = 0;

    virtual void addProcess(std::shared_ptr<Process> process) = 0;

    virtual std::vector<std::shared_ptr<Process>> getAllProcesses() const = 0;

    virtual bool allCoresFree() = 0;
    virtual bool noProcessFinished() = 0;

private:
    virtual void schedulerLoop() = 0;
    virtual void workerLoop(int coreId) = 0;

    virtual bool hasFreeCore() = 0;
};
