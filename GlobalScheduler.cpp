#include <algorithm>
#include <iostream>

#include "GlobalScheduler.h"

GlobalScheduler::GlobalScheduler(const SystemConfig& config) {
    // Instantiate each scheduler
    schedulers["fcfs"] = std::make_shared<FCFSScheduler>(config);
    schedulers["rr"] = std::make_shared<RRScheduler>(config);

    // Set Current Scheduler
    std::string schedName = config.scheduler;
    std::transform(schedName.begin(), schedName.end(), schedName.begin(), ::tolower);

    if (schedulers.find(schedName) != schedulers.end()) {
        currentScheduler = schedulers[schedName];
    }
    else {
        std::cerr << "Warning: Unknown scheduler '" << config.scheduler << "'. Falling back to FCFS.\n";
        currentScheduler = schedulers["fcfs"];
    }
}

void GlobalScheduler::initialize(const SystemConfig& config) {
    if (!sharedInstance)
        sharedInstance = new GlobalScheduler(config);
}

GlobalScheduler* GlobalScheduler::getInstance() {
    return sharedInstance;
}

void GlobalScheduler::destroy() {
    delete sharedInstance;
    sharedInstance = nullptr;
}

void GlobalScheduler::start() {
    running = true;
    if (currentScheduler) currentScheduler->start();
}

void GlobalScheduler::stop() {
    running = false;
    if (currentScheduler) currentScheduler->stop();
}

void GlobalScheduler::addProcess(std::shared_ptr<Process> process) {
    if (currentScheduler) currentScheduler->addProcess(process);
    else std::cerr << "Error: No scheduler is currently set.\n";
}

std::vector<std::shared_ptr<Process>> GlobalScheduler::getAllProcesses() const {
    if (currentScheduler) return currentScheduler->getAllProcesses();
    else return {};
}

void GlobalScheduler::setCurrentScheduler(std::string name) {
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if (schedulers.find(name) != schedulers.end()) {
        currentScheduler = schedulers[name];
        currentScheduler->start();
    }
    else {
        std::cerr << "Scheduler '" << name << "' does not exist.\n";
    }
}

bool GlobalScheduler::allCoresFree() const {
    return currentScheduler ? currentScheduler->allCoresFree() : false;
}

bool GlobalScheduler::noProcessFinished() const {
    return currentScheduler ? currentScheduler->noProcessFinished() : false;
}

bool GlobalScheduler::isRunning() {
    return running;
}

std::vector<Core*> GlobalScheduler::getCores() const {
    return currentScheduler->getCores();
}

int GlobalScheduler::getNumCPUCycles() const {
    return currentScheduler->getNumCPUCycles();
}