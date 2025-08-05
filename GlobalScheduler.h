#pragma once  

#include <memory>
#include <unordered_map>

#include "SystemConfig.h"
#include "FCFSScheduler.h"
#include "RRScheduler.h"

class Scheduler;

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
	void notifyScheduler();
    std::vector<Core*> getCores() const;

    bool allCoresFree() const;
    bool noProcessFinished() const;
    bool isRunning();

    void incrementIdleTicks() { ++idleTicks; }
    void incrementActiveTicks() { ++activeTicks; }

    uint64_t getIdleTicks() const { return idleTicks; }
    uint64_t getActiveTicks() const { return activeTicks; }
    uint64_t getTotalTicks() const { return idleTicks + activeTicks; }

private:
    GlobalScheduler(const SystemConfig& config);

    inline static GlobalScheduler* sharedInstance = nullptr;
    bool running = false;

    std::unordered_map<std::string, std::shared_ptr<Scheduler>> schedulers;
    std::shared_ptr<Scheduler> currentScheduler;

    uint64_t idleTicks = 0;
    uint64_t activeTicks = 0;
};