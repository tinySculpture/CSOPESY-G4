#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <windows.h>

#include "FCFSScheduler.h"
#include "GlobalScheduler.h"



GlobalScheduler::GlobalScheduler()
{
    // Initialize schedulers
    schedulers["FCFS"] = std::make_shared<FCFSScheduler>();
    currentScheduler = schedulers["FCFS"];
}



GlobalScheduler* GlobalScheduler::sharedInstance = nullptr;
void GlobalScheduler::initialize() {
    sharedInstance = new GlobalScheduler();
}



void GlobalScheduler::start() {
    running = true;
    if (currentScheduler) {
        currentScheduler->start();
    }
}



void GlobalScheduler::stop() {
    running = false;
    if (currentScheduler) {
        currentScheduler->stop();
    }
}



void GlobalScheduler::destroy() {
    delete sharedInstance;
}



void GlobalScheduler::setCurrentScheduler(std::string name) {
    if (schedulers.contains(name))
    {
        currentScheduler = schedulers[name]; // Switch to the new layout
        currentScheduler->start(); // Render the new layout
    }
    else
    {
        std::cerr << "Error: Scheduler '" << name << "' does not exist." << std::endl;
    }
}



void GlobalScheduler::addProcess(std::shared_ptr<Process> process) {
    if (currentScheduler) {
        currentScheduler->addProcess(process);
    } else {
        std::cerr << "Error: No scheduler is currently set." << std::endl;
	}
}



std::vector<std::shared_ptr<Process>> GlobalScheduler::getAllProcesses() const {
    if (currentScheduler) {
        return currentScheduler->getAllProcesses();
    } else {
        std::cerr << "Error: No scheduler is currently set." << std::endl;
		return std::vector<std::shared_ptr<Process>>();
	}
}



GlobalScheduler* GlobalScheduler::getInstance() {
    return sharedInstance;
}



bool GlobalScheduler::isRunning() {
    return running;
}


bool GlobalScheduler::allCoresFree() const {
    if (currentScheduler) {
        return currentScheduler->allCoresFree();
    } else {
        std::cerr << "Error: No scheduler is currently set." << std::endl;
        return false;
	}
}

bool GlobalScheduler::noProcessFinished() const
{
    if (currentScheduler) {
        return currentScheduler->noProcessFinished();
    }

    std::cerr << "Error: No scheduler is currently set." << std::endl;
    return false;
}