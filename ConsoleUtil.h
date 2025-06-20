#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Process.h"

namespace ConsoleUtil
{
	std::vector<std::string> tokenizeInput(const std::string input);

	std::shared_ptr<Process> findProcessByName(const std::string& name);

	std::string generateTimestamp();

	std::string truncateLongNames(std::string name);
}