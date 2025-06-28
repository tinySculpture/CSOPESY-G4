#pragma once

#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include "GlobalScheduler.h"
#include "ConsoleUtil.h"
#include "Process.h"

namespace ConsoleUtil
{
	std::vector<std::string> tokenizeInput(const std::string input);
	std::shared_ptr<Process> findProcessByName(const std::string& name);
	std::string generateTimestamp();
	std::string truncateLongNames(std::string name);
}