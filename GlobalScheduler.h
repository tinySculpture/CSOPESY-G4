#pragma once  

#include <memory>
#include <unordered_map>

#include "SystemConfig.h"
#include "FCFSScheduler.h"
#include "RRScheduler.h"
#include "Scheduler.h"  


class GlobalScheduler {
public:
    static void initialize(const SystemConfig& config);
    static GlobalScheduler* getInstance();
    static void destroy();

    void start();
    void stop();

    void addProcess(std::shared_ptr<Process> process);
    std::vector<std::shared_ptr<Process>> getAllProcesses() const;
    void setCurrentScheduler(std::string name);

    bool allCoresFree() const;
    bool noProcessFinished() const;
    bool isRunning();

private:
    GlobalScheduler(const SystemConfig& config);

    inline static GlobalScheduler* sharedInstance = nullptr;
    bool running = false;

    std::unordered_map<std::string, std::shared_ptr<Scheduler>> schedulers;
    std::shared_ptr<Scheduler> currentScheduler;
};