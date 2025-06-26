#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Process.h"

class Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual std::vector<std::shared_ptr<Process>> getAllProcesses() const = 0;

    virtual bool allCoresFree() = 0;
    virtual bool noProcessFinished() = 0;
};