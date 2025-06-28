#pragma once

#include <memory>
#include <vector>
#include <string>

class Process;


/**
 * @class Scheduler
 * @brief Abstract base class for scheduling policies.
 *
 * Subclasses must implement methods to control scheduler lifecycle,
 * accept new processes, and provide information about scheduling state.
 */
class Scheduler {
public:
    Scheduler() {}
    virtual ~Scheduler() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual std::vector<std::shared_ptr<Process>> getAllProcesses() const = 0;

    virtual bool allCoresFree() = 0;
    virtual bool noProcessFinished() = 0;
};