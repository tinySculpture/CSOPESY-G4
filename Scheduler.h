#pragma once

#include "Process.h"

class Scheduler {
public:
	void addProcess(const Process* process);
	void sortReadyQueue();
	void runScheduler();
};